#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Resources/ResourceTypes.h"
#include "ResourceInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnResourceAmountChanged, FName, ResourceId, int32, NewAmount, int32, Delta);

/**
 * Tick-free, integer resource inventory shared by players, workers and buildings.
 * Mutating calls are authoritative and broadcast one event per changed resource.
 */
UCLASS(ClassGroup = (Settlement), meta = (BlueprintSpawnableComponent))
class SETTLEMENTGAME_API UResourceInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UResourceInventoryComponent();

    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetResourceAmount(FName ResourceId) const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetTotalUnits() const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetFreeCapacity() const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool CanAddResource(FName ResourceId, int32 Amount) const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool HasResource(FName ResourceId, int32 Amount) const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool HasResources(const TArray<FResourceAmount>& RequiredResources) const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool CanAddResources(const TArray<FResourceAmount>& ResourcesToAdd) const;

    /** Returns the amount actually accepted. */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 AddResource(FName ResourceId, int32 Amount);

    /** Returns the amount actually removed. */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 RemoveResource(FName ResourceId, int32 Amount);

    /** Atomic all-or-nothing operation. */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveResources(const TArray<FResourceAmount>& RequiredResources);

    /** Atomic with respect to capacity: either all entries fit or none are added. */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddResources(const TArray<FResourceAmount>& ResourcesToAdd);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 TransferResourceTo(UResourceInventoryComponent* Target, FName ResourceId, int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 TransferAllTo(UResourceInventoryComponent* Target);

    UFUNCTION(BlueprintPure, Category = "Inventory")
    TArray<FResourceAmount> GetContents() const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    FString ToDebugString() const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetCapacity(int32 NewCapacity);

    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetCapacity() const { return Capacity; }

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnResourceAmountChanged OnResourceAmountChanged;

protected:
    /** Negative means unlimited. Capacity is measured in resource units in this slice. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    int32 Capacity = -1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TMap<FName, int32> Resources;
};
