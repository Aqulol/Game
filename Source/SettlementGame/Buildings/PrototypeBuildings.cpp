#include "Buildings/PrototypeBuildings.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Inventory/ResourceInventoryComponent.h"
#include "Production/ProductionComponent.h"
#include "Resources/ResourceTypes.h"
#include "Workers/SettlementWorkerCharacter.h"

AStorageBuilding::AStorageBuilding()
{
    BuildingId = TEXT("Building.Warehouse");
    DisplayName = NSLOCTEXT("Settlement", "Warehouse", "Warehouse");
    BuildCost = {
        FResourceAmount(FSettlementResourceIds::Wood, 10),
        FResourceAmount(FSettlementResourceIds::Stone, 5)
    };
    Footprint = FVector(400.0f, 320.0f, 240.0f);
    MainMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
    MainMesh->SetRelativeScale3D(FVector(4.0f, 3.2f, 2.4f));
    Inventory->SetCapacity(500);
}

AHouseBuilding::AHouseBuilding()
{
    BuildingId = TEXT("Building.House");
    DisplayName = NSLOCTEXT("Settlement", "House", "House");
    BuildCost = {
        FResourceAmount(FSettlementResourceIds::Wood, 8),
        FResourceAmount(FSettlementResourceIds::Stone, 3)
    };
    Footprint = FVector(360.0f, 300.0f, 300.0f);
    MainMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
    MainMesh->SetRelativeScale3D(FVector(3.6f, 3.0f, 3.0f));
    Inventory->SetCapacity(20);
    WorkerClass = ASettlementWorkerCharacter::StaticClass();
}

void AHouseBuilding::BeginPlay()
{
    Super::BeginPlay();
    if (!bSpawnResident || IsPreview() || !HasAuthority() || !WorkerClass)
    {
        return;
    }

    const FVector SpawnLocation = GetActorLocation() + GetActorRightVector() * 260.0f + FVector(0.0f, 0.0f, 100.0f);
    const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);
    ASettlementWorkerCharacter* Worker = GetWorld()->SpawnActorDeferred<ASettlementWorkerCharacter>(
        WorkerClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
    if (Worker)
    {
        Worker->SetHome(this);
        Worker->FinishSpawning(SpawnTransform);
    }
}

APlankPlatformBuilding::APlankPlatformBuilding()
{
    BuildingId = TEXT("Building.PlankPlatform");
    DisplayName = NSLOCTEXT("Settlement", "PlankPlatform", "Plank platform");
    BuildCost = {FResourceAmount(FSettlementResourceIds::Planks, 4)};
    Footprint = FVector(300.0f, 300.0f, 40.0f);
    MainMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 20.0f));
    MainMesh->SetRelativeScale3D(FVector(3.0f, 3.0f, 0.4f));
    Inventory->SetCapacity(0);
}

ASawmillBuilding::ASawmillBuilding()
{
    BuildingId = TEXT("Building.Sawmill");
    DisplayName = NSLOCTEXT("Settlement", "Sawmill", "Sawmill");
    BuildCost = {
        FResourceAmount(FSettlementResourceIds::Wood, 12),
        FResourceAmount(FSettlementResourceIds::Stone, 6)
    };
    Footprint = FVector(500.0f, 380.0f, 220.0f);
    MainMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
    MainMesh->SetRelativeScale3D(FVector(5.0f, 3.8f, 2.2f));
    Inventory->SetCapacity(100);

    Production = CreateDefaultSubobject<UProductionComponent>(TEXT("Production"));
    FProductionRecipeSpec SawmillRecipe;
    SawmillRecipe.RecipeId = TEXT("Recipe.WoodToPlanks");
    SawmillRecipe.DisplayName = NSLOCTEXT("Settlement", "WoodToPlanks", "Saw planks");
    SawmillRecipe.Inputs = {FResourceAmount(FSettlementResourceIds::Wood, 2)};
    SawmillRecipe.Outputs = {FResourceAmount(FSettlementResourceIds::Planks, 2)};
    SawmillRecipe.DurationSeconds = 4.0f;
    Production->SetInlineRecipe(SawmillRecipe);
}
