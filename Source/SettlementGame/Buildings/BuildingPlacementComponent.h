#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Resources/ResourceTypes.h"
#include "BuildingPlacementComponent.generated.h"

class ABuildingBase;
class UBuildingDefinition;
class UResourceInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildingModeChanged, bool, bIsBuilding);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBuildingPlaced, ABuildingBase*, Building, bool, bSuccess);

/**
 * Player-side placement mode. Only this component ticks while a preview is active.
 * Costs can be paid from the player and any constructed warehouse.
 */
UCLASS(ClassGroup = (Settlement), meta = (BlueprintSpawnableComponent))
class SETTLEMENTGAME_API UBuildingPlacementComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBuildingPlacementComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Building")
    void SelectBuildingClass(TSubclassOf<ABuildingBase> BuildingClass);

    UFUNCTION(BlueprintCallable, Category = "Building")
    void SelectBuildingDefinition(UBuildingDefinition* Definition);

    UFUNCTION(BlueprintCallable, Category = "Building")
    bool TryPlaceBuilding();

    UFUNCTION(BlueprintCallable, Category = "Building")
    void CancelPlacement();

    UFUNCTION(BlueprintPure, Category = "Building")
    bool IsPlacementActive() const { return SelectedBuildingClass != nullptr; }

    UFUNCTION(BlueprintPure, Category = "Building")
    bool IsCurrentLocationValid() const { return bCurrentLocationValid; }

    UPROPERTY(BlueprintAssignable, Category = "Building")
    FOnBuildingModeChanged OnBuildingModeChanged;

    UPROPERTY(BlueprintAssignable, Category = "Building")
    FOnBuildingPlaced OnBuildingPlaced;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building", meta = (ClampMin = "100.0"))
    float PlacementDistance = 1600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building", meta = (ClampMin = "1.0"))
    float GridSize = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MinimumGroundNormalZ = 0.85f;

private:
    UPROPERTY(Transient)
    TSubclassOf<ABuildingBase> SelectedBuildingClass;

    UPROPERTY(Transient)
    TObjectPtr<UBuildingDefinition> SelectedDefinition;

    UPROPERTY(Transient)
    TObjectPtr<ABuildingBase> PreviewActor;

    FTransform CurrentPlacementTransform;
    bool bCurrentLocationValid = false;

    void CreatePreview();
    void UpdatePreview();
    TArray<FResourceAmount> GetSelectedCost() const;
    FVector GetSelectedFootprint() const;
    bool CanAfford(const TArray<FResourceAmount>& Cost) const;
    bool SpendResources(const TArray<FResourceAmount>& Cost, TArray<TPair<TWeakObjectPtr<UResourceInventoryComponent>, FResourceAmount>>& OutDebits);
    void RefundResources(const TArray<TPair<TWeakObjectPtr<UResourceInventoryComponent>, FResourceAmount>>& Debits);
    TArray<UResourceInventoryComponent*> GetPaymentInventories() const;
};
