#include "Interaction/InteractionComponent.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Interaction/Interactable.h"
#include "Engine/World.h"
#include "TimerManager.h"

UInteractionComponent::UInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::BeginPlay()
{
    Super::BeginPlay();
    GetWorld()->GetTimerManager().SetTimer(ScanTimer, this, &UInteractionComponent::RefreshFocus, ScanInterval, true, 0.0f);
}

void UInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(ScanTimer);
    }
    Super::EndPlay(EndPlayReason);
}

void UInteractionComponent::RefreshFocus()
{
    const APawn* PawnOwner = Cast<APawn>(GetOwner());
    const APlayerController* PlayerController = PawnOwner ? Cast<APlayerController>(PawnOwner->GetController()) : nullptr;
    if (!PlayerController)
    {
        SetFocusedActor(nullptr);
        return;
    }

    FVector ViewLocation;
    FRotator ViewRotation;
    PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

    FHitResult Hit;
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SettlementInteraction), false, GetOwner());
    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        ViewLocation,
        ViewLocation + ViewRotation.Vector() * TraceDistance,
        ECC_Visibility,
        QueryParams);

    AActor* Candidate = bHit ? Hit.GetActor() : nullptr;
    if (!IsValid(Candidate) || !Candidate->GetClass()->ImplementsInterface(UInteractable::StaticClass()) ||
        !IInteractable::Execute_CanInteract(Candidate, GetOwner()))
    {
        Candidate = nullptr;
    }
    SetFocusedActor(Candidate);
}

void UInteractionComponent::TryInteract()
{
    RefreshFocus();
    if (IsValid(FocusedActor) && IInteractable::Execute_CanInteract(FocusedActor, GetOwner()))
    {
        IInteractable::Execute_Interact(FocusedActor, GetOwner());
        RefreshFocus();
    }
}

void UInteractionComponent::SetFocusedActor(AActor* NewActor)
{
    if (FocusedActor == NewActor)
    {
        return;
    }
    AActor* Previous = FocusedActor;
    FocusedActor = NewActor;
    OnFocusedInteractableChanged.Broadcast(Previous, NewActor);
}
