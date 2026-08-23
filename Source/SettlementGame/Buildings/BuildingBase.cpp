#include "Buildings/BuildingBase.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Inventory/ResourceInventoryComponent.h"
#include "UObject/ConstructorHelpers.h"

ABuildingBase::ABuildingBase()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(SceneRoot);

    MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainMesh"));
    MainMesh->SetupAttachment(SceneRoot);
    MainMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        MainMesh->SetStaticMesh(CubeMesh.Object);
    }

    Inventory = CreateDefaultSubobject<UResourceInventoryComponent>(TEXT("Inventory"));
    Inventory->SetCapacity(100);
}

bool ABuildingBase::CanInteract_Implementation(AActor* Interactor) const
{
    return !bPreview && IsValid(Interactor) && Interactor->FindComponentByClass<UResourceInventoryComponent>() != nullptr;
}

FText ABuildingBase::GetInteractionPrompt_Implementation(AActor* Interactor) const
{
    return FText::FromString(FString::Printf(
        TEXT("%s - deposit carried resources [%s]"),
        *DisplayName.ToString(),
        *Inventory->ToDebugString()));
}

void ABuildingBase::Interact_Implementation(AActor* Interactor)
{
    UResourceInventoryComponent* Source = IsValid(Interactor)
        ? Interactor->FindComponentByClass<UResourceInventoryComponent>()
        : nullptr;
    if (Source)
    {
        Source->TransferAllTo(Inventory);
    }
}

void ABuildingBase::SetPreview(const bool bNewPreview)
{
    bPreview = bNewPreview;
    SetActorEnableCollision(!bPreview);
    MainMesh->SetRenderCustomDepth(bPreview);
    MainMesh->SetCastShadow(!bPreview);
}
