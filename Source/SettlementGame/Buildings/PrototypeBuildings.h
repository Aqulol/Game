#pragma once

#include "CoreMinimal.h"
#include "Buildings/BuildingBase.h"
#include "PrototypeBuildings.generated.h"

class ASettlementWorkerCharacter;
class UProductionComponent;

UCLASS(Blueprintable)
class SETTLEMENTGAME_API AStorageBuilding : public ABuildingBase
{
    GENERATED_BODY()

public:
    AStorageBuilding();
};

UCLASS(Blueprintable)
class SETTLEMENTGAME_API AHouseBuilding : public ABuildingBase
{
    GENERATED_BODY()

public:
    AHouseBuilding();
    virtual void BeginPlay() override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Workers")
    TSubclassOf<ASettlementWorkerCharacter> WorkerClass;

    UPROPERTY(EditDefaultsOnly, Category = "Workers")
    bool bSpawnResident = true;
};

/** Final acceptance object proving that produced planks feed construction. */
UCLASS(Blueprintable)
class SETTLEMENTGAME_API APlankPlatformBuilding : public ABuildingBase
{
    GENERATED_BODY()

public:
    APlankPlatformBuilding();
};

UCLASS(Blueprintable)
class SETTLEMENTGAME_API ASawmillBuilding : public ABuildingBase
{
    GENERATED_BODY()

public:
    ASawmillBuilding();

    UFUNCTION(BlueprintPure, Category = "Production")
    UProductionComponent* GetProductionComponent() const { return Production; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UProductionComponent> Production;
};
