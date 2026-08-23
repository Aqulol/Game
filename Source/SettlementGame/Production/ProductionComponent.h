#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Production/ProductionTypes.h"
#include "ProductionComponent.generated.h"

class UProductionRecipeDefinition;
class UResourceInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProductionStateChanged, EProductionState, OldState, EProductionState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProductionBatchCompleted, FName, RecipeId);

/** Interval-based producer. Inputs are reserved at batch start and outputs are added at completion. */
UCLASS(ClassGroup = (Settlement), meta = (BlueprintSpawnableComponent))
class SETTLEMENTGAME_API UProductionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UProductionComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION(BlueprintCallable, Category = "Production")
    void StartProduction();

    UFUNCTION(BlueprintCallable, Category = "Production")
    void StopProduction();

    UFUNCTION(BlueprintCallable, Category = "Production")
    void SetInlineRecipe(const FProductionRecipeSpec& NewRecipe);

    UFUNCTION(BlueprintPure, Category = "Production")
    FProductionRecipeSpec GetActiveRecipe() const;

    UFUNCTION(BlueprintPure, Category = "Production")
    EProductionState GetProductionState() const { return State; }

    UFUNCTION(BlueprintPure, Category = "Production")
    float GetRemainingSeconds() const { return RemainingSeconds; }

    UPROPERTY(BlueprintAssignable, Category = "Production")
    FOnProductionStateChanged OnProductionStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Production")
    FOnProductionBatchCompleted OnProductionBatchCompleted;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Production")
    TObjectPtr<UProductionRecipeDefinition> RecipeDefinition;

    /** Used by the native sawmill and as a fallback when no Data Asset is assigned. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Production")
    FProductionRecipeSpec InlineRecipe;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Production")
    bool bAutoStart = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Production", meta = (ClampMin = "0.05"))
    float SimulationInterval = 0.25f;

private:
    UPROPERTY(Transient)
    TObjectPtr<UResourceInventoryComponent> Inventory;

    UPROPERTY(VisibleInstanceOnly, Category = "Production")
    EProductionState State = EProductionState::Stopped;

    float RemainingSeconds = 0.0f;
    FTimerHandle SimulationTimer;

    void SimulateProduction();
    void SetState(EProductionState NewState);
    bool HasRoomAfterConsumingInputs(const FProductionRecipeSpec& Recipe) const;
};
