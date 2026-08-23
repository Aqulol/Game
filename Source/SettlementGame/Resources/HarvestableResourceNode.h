#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Interactable.h"
#include "HarvestableResourceNode.generated.h"

class UStaticMeshComponent;

/** A lightweight aggregate resource node. One tree/rock is one actor, not one actor per item. */
UCLASS(Blueprintable)
class SETTLEMENTGAME_API AHarvestableResourceNode : public AActor, public IInteractable
{
    GENERATED_BODY()

public:
    AHarvestableResourceNode();

    UFUNCTION(BlueprintCallable, Category = "Resource")
    void Configure(FName NewResourceId, int32 NewAmount);

    /** Removes and returns up to RequestedAmount. Used by workers. */
    UFUNCTION(BlueprintCallable, Category = "Resource")
    int32 Harvest(int32 RequestedAmount);

    UFUNCTION(BlueprintPure, Category = "Resource")
    int32 GetRemainingAmount() const { return RemainingAmount; }

    UFUNCTION(BlueprintPure, Category = "Resource")
    FName GetResourceId() const { return ResourceId; }

    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual FText GetInteractionPrompt_Implementation(AActor* Interactor) const override;
    virtual void Interact_Implementation(AActor* Interactor) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource")
    FName ResourceId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource", meta = (ClampMin = "0"))
    int32 RemainingAmount = 20;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource", meta = (ClampMin = "1"))
    int32 PlayerHarvestAmount = 3;

private:
    void RefreshAppearance();
};
