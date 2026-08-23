#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Interactable.h"
#include "Resources/ResourceTypes.h"
#include "BuildingBase.generated.h"

class UResourceInventoryComponent;
class USceneComponent;
class UStaticMeshComponent;

/** Common building shell. Config lives in native defaults, Blueprint subclasses or Building Data Assets. */
UCLASS(Blueprintable)
class SETTLEMENTGAME_API ABuildingBase : public AActor, public IInteractable
{
    GENERATED_BODY()

public:
    ABuildingBase();

    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual FText GetInteractionPrompt_Implementation(AActor* Interactor) const override;
    virtual void Interact_Implementation(AActor* Interactor) override;

    UFUNCTION(BlueprintPure, Category = "Building")
    FName GetBuildingId() const { return BuildingId; }

    UFUNCTION(BlueprintPure, Category = "Building")
    FText GetBuildingDisplayName() const { return DisplayName; }

    UFUNCTION(BlueprintPure, Category = "Building")
    TArray<FResourceAmount> GetBuildCost() const { return BuildCost; }

    UFUNCTION(BlueprintPure, Category = "Building")
    FVector GetFootprint() const { return Footprint; }

    UFUNCTION(BlueprintPure, Category = "Building")
    UResourceInventoryComponent* GetInventory() const { return Inventory; }

    UFUNCTION(BlueprintPure, Category = "Building")
    bool IsPreview() const { return bPreview; }

    UFUNCTION(BlueprintCallable, Category = "Building")
    void SetPreview(bool bNewPreview);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> MainMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UResourceInventoryComponent> Inventory;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
    FName BuildingId = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
    TArray<FResourceAmount> BuildCost;

    /** Full box size in centimeters. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
    FVector Footprint = FVector(300.0f, 300.0f, 200.0f);

private:
    UPROPERTY(Transient)
    bool bPreview = false;
};
