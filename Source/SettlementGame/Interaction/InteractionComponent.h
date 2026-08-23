#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFocusedInteractableChanged, AActor*, PreviousActor, AActor*, NewActor);

/** Performs a throttled camera trace rather than ticking every frame. */
UCLASS(ClassGroup = (Settlement), meta = (BlueprintSpawnableComponent))
class SETTLEMENTGAME_API UInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInteractionComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void TryInteract();

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void RefreshFocus();

    UFUNCTION(BlueprintPure, Category = "Interaction")
    AActor* GetFocusedActor() const { return FocusedActor; }

    UPROPERTY(BlueprintAssignable, Category = "Interaction")
    FOnFocusedInteractableChanged OnFocusedInteractableChanged;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = "50.0"))
    float TraceDistance = 450.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = "0.02"))
    float ScanInterval = 0.08f;

private:
    UPROPERTY(Transient)
    TObjectPtr<AActor> FocusedActor;

    FTimerHandle ScanTimer;
    void SetFocusedActor(AActor* NewActor);
};
