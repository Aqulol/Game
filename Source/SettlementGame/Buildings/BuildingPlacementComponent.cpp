#include "Buildings/BuildingPlacementComponent.h"

#include "Buildings/BuildingBase.h"
#include "Buildings/PrototypeBuildings.h"
#include "Data/BuildingDefinition.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Inventory/ResourceInventoryComponent.h"
#include "Kismet/GameplayStatics.h"

UBuildingPlacementComponent::UBuildingPlacementComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UBuildingPlacementComponent::BeginPlay()
{
    Super::BeginPlay();
    SetComponentTickEnabled(false);
}

void UBuildingPlacementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CancelPlacement();
    Super::EndPlay(EndPlayReason);
}

void UBuildingPlacementComponent::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    UpdatePreview();
}

void UBuildingPlacementComponent::SelectBuildingClass(const TSubclassOf<ABuildingBase> BuildingClass)
{
    if (!BuildingClass)
    {
        CancelPlacement();
        return;
    }

    if (PreviewActor)
    {
        PreviewActor->Destroy();
        PreviewActor = nullptr;
    }
    SelectedDefinition = nullptr;
    SelectedBuildingClass = BuildingClass;
    CreatePreview();
    SetComponentTickEnabled(true);
    OnBuildingModeChanged.Broadcast(true);
}

void UBuildingPlacementComponent::SelectBuildingDefinition(UBuildingDefinition* Definition)
{
    if (!Definition || !Definition->BuildingClass)
    {
        CancelPlacement();
        return;
    }

    if (PreviewActor)
    {
        PreviewActor->Destroy();
        PreviewActor = nullptr;
    }
    SelectedDefinition = Definition;
    SelectedBuildingClass = Definition->BuildingClass;
    CreatePreview();
    SetComponentTickEnabled(true);
    OnBuildingModeChanged.Broadcast(true);
}

bool UBuildingPlacementComponent::TryPlaceBuilding()
{
    if (!SelectedBuildingClass || !bCurrentLocationValid || !CanAfford(GetSelectedCost()))
    {
        OnBuildingPlaced.Broadcast(nullptr, false);
        return false;
    }

    TArray<TPair<TWeakObjectPtr<UResourceInventoryComponent>, FResourceAmount>> Debits;
    if (!SpendResources(GetSelectedCost(), Debits))
    {
        OnBuildingPlaced.Broadcast(nullptr, false);
        return false;
    }

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = GetOwner();
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    ABuildingBase* Building = GetWorld()->SpawnActor<ABuildingBase>(
        SelectedBuildingClass, CurrentPlacementTransform, SpawnParameters);
    if (!Building)
    {
        RefundResources(Debits);
        OnBuildingPlaced.Broadcast(nullptr, false);
        return false;
    }

    OnBuildingPlaced.Broadcast(Building, true);
    CancelPlacement();
    return true;
}

void UBuildingPlacementComponent::CancelPlacement()
{
    if (PreviewActor)
    {
        PreviewActor->Destroy();
        PreviewActor = nullptr;
    }
    const bool bWasActive = SelectedBuildingClass != nullptr;
    SelectedBuildingClass = nullptr;
    SelectedDefinition = nullptr;
    bCurrentLocationValid = false;
    SetComponentTickEnabled(false);
    if (bWasActive)
    {
        OnBuildingModeChanged.Broadcast(false);
    }
}

void UBuildingPlacementComponent::CreatePreview()
{
    if (!SelectedBuildingClass || !GetWorld())
    {
        return;
    }

    const FTransform InitialTransform(FRotator::ZeroRotator, GetOwner()->GetActorLocation());
    PreviewActor = GetWorld()->SpawnActorDeferred<ABuildingBase>(
        SelectedBuildingClass,
        InitialTransform,
        GetOwner(),
        nullptr,
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    if (PreviewActor)
    {
        PreviewActor->SetPreview(true);
        UGameplayStatics::FinishSpawningActor(PreviewActor, InitialTransform);
    }
    UpdatePreview();
}

void UBuildingPlacementComponent::UpdatePreview()
{
    const APawn* PawnOwner = Cast<APawn>(GetOwner());
    const APlayerController* Controller = PawnOwner ? Cast<APlayerController>(PawnOwner->GetController()) : nullptr;
    if (!Controller || !SelectedBuildingClass)
    {
        bCurrentLocationValid = false;
        return;
    }

    FVector ViewLocation;
    FRotator ViewRotation;
    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(BuildingPlacementGround), false, GetOwner());
    if (PreviewActor)
    {
        QueryParams.AddIgnoredActor(PreviewActor);
    }

    FHitResult GroundHit;
    const bool bHitGround = GetWorld()->LineTraceSingleByChannel(
        GroundHit,
        ViewLocation,
        ViewLocation + ViewRotation.Vector() * PlacementDistance,
        ECC_Visibility,
        QueryParams);

    if (!bHitGround)
    {
        bCurrentLocationValid = false;
        if (PreviewActor)
        {
            PreviewActor->SetActorHiddenInGame(true);
        }
        return;
    }

    FVector Location = GroundHit.ImpactPoint;
    Location.X = FMath::GridSnap(Location.X, GridSize);
    Location.Y = FMath::GridSnap(Location.Y, GridSize);
    const float Yaw = FMath::GridSnap(Controller->GetControlRotation().Yaw, 90.0f);
    CurrentPlacementTransform = FTransform(FRotator(0.0f, Yaw, 0.0f), Location);

    const FVector Footprint = GetSelectedFootprint();
    const FVector QueryCenter = Location + FVector(0.0f, 0.0f, Footprint.Z * 0.5f + 2.0f);
    FCollisionObjectQueryParams ObjectQuery;
    ObjectQuery.AddObjectTypesToQuery(ECC_WorldDynamic);
    FCollisionQueryParams OverlapParams(SCENE_QUERY_STAT(BuildingPlacementOverlap), false, GetOwner());
    if (PreviewActor)
    {
        OverlapParams.AddIgnoredActor(PreviewActor);
    }
    TArray<FOverlapResult> Overlaps;
    const bool bOverlaps = GetWorld()->OverlapMultiByObjectType(
        Overlaps,
        QueryCenter,
        CurrentPlacementTransform.GetRotation(),
        ObjectQuery,
        FCollisionShape::MakeBox(Footprint * 0.5f * 0.95f),
        OverlapParams);

    bCurrentLocationValid = GroundHit.ImpactNormal.Z >= MinimumGroundNormalZ && !bOverlaps;
    if (PreviewActor)
    {
        PreviewActor->SetActorHiddenInGame(false);
        PreviewActor->SetActorTransform(CurrentPlacementTransform);
    }
}

TArray<FResourceAmount> UBuildingPlacementComponent::GetSelectedCost() const
{
    if (SelectedDefinition)
    {
        return SelectedDefinition->BuildCost;
    }
    const ABuildingBase* Defaults = SelectedBuildingClass ? SelectedBuildingClass->GetDefaultObject<ABuildingBase>() : nullptr;
    return Defaults ? Defaults->GetBuildCost() : TArray<FResourceAmount>();
}

FVector UBuildingPlacementComponent::GetSelectedFootprint() const
{
    if (SelectedDefinition)
    {
        return SelectedDefinition->Footprint;
    }
    const ABuildingBase* Defaults = SelectedBuildingClass ? SelectedBuildingClass->GetDefaultObject<ABuildingBase>() : nullptr;
    return Defaults ? Defaults->GetFootprint() : FVector(300.0f);
}

bool UBuildingPlacementComponent::CanAfford(const TArray<FResourceAmount>& Cost) const
{
    TMap<FName, int32> Available;
    for (const UResourceInventoryComponent* Inventory : GetPaymentInventories())
    {
        if (!Inventory)
        {
            continue;
        }
        for (const FResourceAmount& Entry : Inventory->GetContents())
        {
            Available.FindOrAdd(Entry.ResourceId) += Entry.Amount;
        }
    }

    for (const FResourceAmount& Entry : Cost)
    {
        if (!Entry.IsValid() || Available.FindRef(Entry.ResourceId) < Entry.Amount)
        {
            return false;
        }
        Available.FindOrAdd(Entry.ResourceId) -= Entry.Amount;
    }
    return true;
}

bool UBuildingPlacementComponent::SpendResources(
    const TArray<FResourceAmount>& Cost,
    TArray<TPair<TWeakObjectPtr<UResourceInventoryComponent>, FResourceAmount>>& OutDebits)
{
    OutDebits.Reset();
    if (!CanAfford(Cost))
    {
        return false;
    }

    const TArray<UResourceInventoryComponent*> Inventories = GetPaymentInventories();
    for (const FResourceAmount& CostEntry : Cost)
    {
        int32 Remaining = CostEntry.Amount;
        for (UResourceInventoryComponent* Inventory : Inventories)
        {
            if (!Inventory || Remaining <= 0)
            {
                continue;
            }
            const int32 Removed = Inventory->RemoveResource(CostEntry.ResourceId, Remaining);
            if (Removed > 0)
            {
                OutDebits.Emplace(Inventory, FResourceAmount(CostEntry.ResourceId, Removed));
                Remaining -= Removed;
            }
        }
        if (Remaining > 0)
        {
            RefundResources(OutDebits);
            OutDebits.Reset();
            return false;
        }
    }
    return true;
}

void UBuildingPlacementComponent::RefundResources(
    const TArray<TPair<TWeakObjectPtr<UResourceInventoryComponent>, FResourceAmount>>& Debits)
{
    for (const TPair<TWeakObjectPtr<UResourceInventoryComponent>, FResourceAmount>& Debit : Debits)
    {
        if (Debit.Key.IsValid())
        {
            Debit.Key->AddResource(Debit.Value.ResourceId, Debit.Value.Amount);
        }
    }
}

TArray<UResourceInventoryComponent*> UBuildingPlacementComponent::GetPaymentInventories() const
{
    TArray<UResourceInventoryComponent*> Result;
    if (UResourceInventoryComponent* PlayerInventory = GetOwner()->FindComponentByClass<UResourceInventoryComponent>())
    {
        Result.Add(PlayerInventory);
    }

    for (TActorIterator<AStorageBuilding> It(GetWorld()); It; ++It)
    {
        if (!It->IsPreview() && It->GetInventory())
        {
            Result.Add(It->GetInventory());
        }
    }
    return Result;
}
