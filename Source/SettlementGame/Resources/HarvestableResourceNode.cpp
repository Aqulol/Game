#include "Resources/HarvestableResourceNode.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Inventory/ResourceInventoryComponent.h"
#include "Resources/ResourceTypes.h"
#include "UObject/ConstructorHelpers.h"

AHarvestableResourceNode::AHarvestableResourceNode()
{
    PrimaryActorTick.bCanEverTick = false;
    ResourceId = FSettlementResourceIds::Wood;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ResourceMesh"));
    SetRootComponent(Mesh);
    Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        Mesh->SetStaticMesh(CubeMesh.Object);
    }
    RefreshAppearance();
}

void AHarvestableResourceNode::Configure(const FName NewResourceId, const int32 NewAmount)
{
    ResourceId = NewResourceId;
    RemainingAmount = FMath::Max(0, NewAmount);
    RefreshAppearance();
}

int32 AHarvestableResourceNode::Harvest(const int32 RequestedAmount)
{
    if (RequestedAmount <= 0 || RemainingAmount <= 0)
    {
        return 0;
    }
    const int32 Harvested = FMath::Min(RequestedAmount, RemainingAmount);
    RemainingAmount -= Harvested;
    if (RemainingAmount <= 0)
    {
        SetActorEnableCollision(false);
        SetActorHiddenInGame(true);
    }
    return Harvested;
}

bool AHarvestableResourceNode::CanInteract_Implementation(AActor* Interactor) const
{
    return IsValid(Interactor) && RemainingAmount > 0 && Interactor->FindComponentByClass<UResourceInventoryComponent>() != nullptr;
}

FText AHarvestableResourceNode::GetInteractionPrompt_Implementation(AActor* Interactor) const
{
    const FString ShortName = ResourceId == FSettlementResourceIds::Wood ? TEXT("wood") :
        ResourceId == FSettlementResourceIds::Stone ? TEXT("stone") : ResourceId.ToString();
    return FText::FromString(FString::Printf(TEXT("Gather %s (%d left)"), *ShortName, RemainingAmount));
}

void AHarvestableResourceNode::Interact_Implementation(AActor* Interactor)
{
    UResourceInventoryComponent* Inventory = IsValid(Interactor)
        ? Interactor->FindComponentByClass<UResourceInventoryComponent>()
        : nullptr;
    if (!Inventory || RemainingAmount <= 0)
    {
        return;
    }

    const int32 Wanted = FMath::Min(PlayerHarvestAmount, RemainingAmount);
    const int32 Added = Inventory->AddResource(ResourceId, Wanted);
    if (Added > 0)
    {
        Harvest(Added);
    }
}

void AHarvestableResourceNode::RefreshAppearance()
{
    if (!Mesh)
    {
        return;
    }

    if (ResourceId == FSettlementResourceIds::Stone)
    {
        Mesh->SetRelativeScale3D(FVector(1.25f, 1.0f, 0.8f));
        Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f));
    }
    else
    {
        Mesh->SetRelativeScale3D(FVector(0.7f, 0.7f, 3.5f));
        Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, 175.0f));
    }
}
