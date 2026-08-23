#include "Player/SettlementPlayerCharacter.h"

#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Buildings/BuildingPlacementComponent.h"
#include "Buildings/PrototypeBuildings.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "Inventory/ResourceInventoryComponent.h"
#include "Interaction/InteractionComponent.h"

ASettlementPlayerCharacter::ASettlementPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
    FirstPersonCamera->SetRelativeLocation(FVector(-10.0f, 0.0f, 64.0f));
    FirstPersonCamera->bUsePawnControlRotation = true;

    bUseControllerRotationYaw = true;
    Inventory = CreateDefaultSubobject<UResourceInventoryComponent>(TEXT("Inventory"));
    Inventory->SetCapacity(60);
    Interaction = CreateDefaultSubobject<UInteractionComponent>(TEXT("Interaction"));
    BuildingPlacement = CreateDefaultSubobject<UBuildingPlacementComponent>(TEXT("BuildingPlacement"));
}
void ASettlementPlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    const APlayerController* PlayerController =
        Cast<APlayerController>(GetController());

    if (!PlayerController || !IsLocallyControlled())
    {
        return;
    }

    const float ForwardValue =
        (PlayerController->IsInputKeyDown(EKeys::W) ? 1.0f : 0.0f) -
        (PlayerController->IsInputKeyDown(EKeys::S) ? 1.0f : 0.0f);

    const float RightValue =
        (PlayerController->IsInputKeyDown(EKeys::D) ? 1.0f : 0.0f) -
        (PlayerController->IsInputKeyDown(EKeys::A) ? 1.0f : 0.0f);

    MoveForward(ForwardValue);
    MoveRight(RightValue);
}
void ASettlementPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);

  
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);

    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);
    PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &ASettlementPlayerCharacter::InteractPressed);
    PlayerInputComponent->BindAction(TEXT("PlaceBuilding"), IE_Pressed, this, &ASettlementPlayerCharacter::PlacePressed);
    PlayerInputComponent->BindAction(TEXT("CancelBuilding"), IE_Pressed, this, &ASettlementPlayerCharacter::CancelPressed);
    PlayerInputComponent->BindAction(TEXT("SelectWarehouse"), IE_Pressed, this, &ASettlementPlayerCharacter::SelectWarehouse);
    PlayerInputComponent->BindAction(TEXT("SelectHouse"), IE_Pressed, this, &ASettlementPlayerCharacter::SelectHouse);
    PlayerInputComponent->BindAction(TEXT("SelectSawmill"), IE_Pressed, this, &ASettlementPlayerCharacter::SelectSawmill);
    PlayerInputComponent->BindAction(TEXT("SelectPlankPlatform"), IE_Pressed, this, &ASettlementPlayerCharacter::SelectPlankPlatform);
}

void ASettlementPlayerCharacter::MoveForward(const float Value)
{
    if (!FMath::IsNearlyZero(Value))
    {
        AddMovementInput(
            FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::X),
            Value,
            true);
    }
}

void ASettlementPlayerCharacter::MoveRight(const float Value)
{
    if (!FMath::IsNearlyZero(Value))
    {
        AddMovementInput(
            FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Y),
            Value,
            true);
    }
}
void ASettlementPlayerCharacter::InteractPressed()
{
    if (!BuildingPlacement->IsPlacementActive())
    {
        Interaction->TryInteract();
    }
}

void ASettlementPlayerCharacter::PlacePressed()
{
    if (BuildingPlacement->IsPlacementActive())
    {
        BuildingPlacement->TryPlaceBuilding();
    }
}

void ASettlementPlayerCharacter::CancelPressed()
{
    BuildingPlacement->CancelPlacement();
}

void ASettlementPlayerCharacter::SelectWarehouse()
{
    BuildingPlacement->SelectBuildingClass(AStorageBuilding::StaticClass());
}

void ASettlementPlayerCharacter::SelectHouse()
{
    BuildingPlacement->SelectBuildingClass(AHouseBuilding::StaticClass());
}

void ASettlementPlayerCharacter::SelectSawmill()
{
    BuildingPlacement->SelectBuildingClass(ASawmillBuilding::StaticClass());
}

void ASettlementPlayerCharacter::SelectPlankPlatform()
{
    BuildingPlacement->SelectBuildingClass(APlankPlatformBuilding::StaticClass());
}
