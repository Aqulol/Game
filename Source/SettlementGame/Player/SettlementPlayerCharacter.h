#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SettlementPlayerCharacter.generated.h"

class UBuildingPlacementComponent;
class UCameraComponent;
class UInteractionComponent;
class UResourceInventoryComponent;

/** Asset-free first-person pawn. Blueprint subclasses can replace visuals and input later. */
UCLASS(Blueprintable)
class SETTLEMENTGAME_API ASettlementPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ASettlementPlayerCharacter();
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintPure, Category = "Player")
    UResourceInventoryComponent* GetInventory() const { return Inventory; }

    UFUNCTION(BlueprintPure, Category = "Player")
    UInteractionComponent* GetInteractionComponent() const { return Interaction; }

    UFUNCTION(BlueprintPure, Category = "Player")
    UBuildingPlacementComponent* GetBuildingPlacementComponent() const { return BuildingPlacement; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UCameraComponent> FirstPersonCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UResourceInventoryComponent> Inventory;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UInteractionComponent> Interaction;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBuildingPlacementComponent> BuildingPlacement;

private:
    void MoveForward(float Value);
    void MoveRight(float Value);
    void InteractPressed();
    void PlacePressed();
    void CancelPressed();
    void SelectWarehouse();
    void SelectHouse();
    void SelectSawmill();
    void SelectPlankPlatform();
};
