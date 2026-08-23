#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interaction/Interactable.h"
#include "Workers/WorkerTypes.h"
#include "SettlementWorkerCharacter.generated.h"

class ABuildingBase;
class AHarvestableResourceNode;
class AHouseBuilding;
class ASawmillBuilding;
class AStorageBuilding;
class UResourceInventoryComponent;
class UStaticMeshComponent;
class UWorkerJobDefinition;

/** One interval-simulated resident supporting the lumberjack and sawyer vertical-slice jobs. */
UCLASS(Blueprintable)
class SETTLEMENTGAME_API ASettlementWorkerCharacter : public ACharacter, public IInteractable
{
    GENERATED_BODY()

public:
    ASettlementWorkerCharacter();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual FText GetInteractionPrompt_Implementation(AActor* Interactor) const override;
    virtual void Interact_Implementation(AActor* Interactor) override;

    UFUNCTION(BlueprintCallable, Category = "Worker")
    void SetHome(AHouseBuilding* NewHome) { Home = NewHome; }

    UFUNCTION(BlueprintCallable, Category = "Worker")
    bool AssignJob(EWorkerJobBehavior NewJob, ABuildingBase* NewWorkplace = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Worker")
    bool AssignJobDefinition(UWorkerJobDefinition* JobDefinition, ABuildingBase* NewWorkplace = nullptr);

    UFUNCTION(BlueprintPure, Category = "Worker")
    EWorkerJobBehavior GetJob() const { return Job; }

    UFUNCTION(BlueprintPure, Category = "Worker")
    EWorkerTaskState GetTaskState() const { return TaskState; }

    UFUNCTION(BlueprintPure, Category = "Worker")
    FText GetWorkerName() const { return WorkerName; }

    UFUNCTION(BlueprintPure, Category = "Worker")
    FText GetCurrentTaskText() const;

    UFUNCTION(BlueprintPure, Category = "Worker")
    FText GetJobText() const;

    UFUNCTION(BlueprintPure, Category = "Worker")
    float GetEnergy() const { return Energy; }

    UFUNCTION(BlueprintPure, Category = "Worker")
    float GetHunger() const { return Hunger; }

    UFUNCTION(BlueprintPure, Category = "Worker")
    ABuildingBase* GetWorkplace() const { return Workplace; }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Worker")
    FText WorkerName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Worker", meta = (ClampMin = "0", ClampMax = "100"))
    float Hunger = 10.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Worker", meta = (ClampMin = "0", ClampMax = "100"))
    float Energy = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Worker")
    EWorkerJobBehavior Job = EWorkerJobBehavior::None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Worker")
    EWorkerTaskState TaskState = EWorkerTaskState::Idle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UResourceInventoryComponent> CarriedInventory;

    /** Placeholder body so the asset-free prototype visibly contains a resident. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> PlaceholderBody;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Worker|Simulation", meta = (ClampMin = "0.1"))
    float SimulationInterval = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Worker|Simulation", meta = (ClampMin = "1"))
    int32 HarvestUnitsPerTrip = 4;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Worker|Simulation", meta = (ClampMin = "0.1"))
    float HarvestDurationSeconds = 2.0f;

private:
    UPROPERTY(Transient)
    TObjectPtr<AHouseBuilding> Home;

    UPROPERTY(Transient)
    TObjectPtr<ABuildingBase> Workplace;

    UPROPERTY(Transient)
    TObjectPtr<AActor> TaskTarget;

    FTimerHandle SimulationTimer;
    float TaskTimeRemaining = 0.0f;
    float AcceptanceRadius = 125.0f;

    void SimulateWorker();
    void SimulateLumberjack();
    void SimulateSawyer();
    void BeginRest();
    void SetTask(EWorkerTaskState NewState, AActor* NewTarget = nullptr);
    bool MoveToTarget(AActor* NewTarget, EWorkerTaskState MovingState);
    bool HasReachedTarget() const;
    AHarvestableResourceNode* FindNearestWoodNode() const;
    AStorageBuilding* FindNearestStorage(FName RequiredResource = NAME_None, int32 RequiredAmount = 0) const;
    ASawmillBuilding* FindNearestSawmill() const;
};
