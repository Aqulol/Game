#include "Production/ProductionComponent.h"

#include "Buildings/BuildingBase.h"
#include "Data/ProductionRecipeDefinition.h"
#include "Engine/World.h"
#include "Inventory/ResourceInventoryComponent.h"
#include "TimerManager.h"

UProductionComponent::UProductionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UProductionComponent::BeginPlay()
{
    Super::BeginPlay();
    Inventory = GetOwner()->FindComponentByClass<UResourceInventoryComponent>();
    const ABuildingBase* BuildingOwner = Cast<ABuildingBase>(GetOwner());
    if (bAutoStart && (!BuildingOwner || !BuildingOwner->IsPreview()))
    {
        StartProduction();
    }
}

void UProductionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    StopProduction();
    Super::EndPlay(EndPlayReason);
}

void UProductionComponent::StartProduction()
{
    if (!Inventory)
    {
        Inventory = GetOwner() ? GetOwner()->FindComponentByClass<UResourceInventoryComponent>() : nullptr;
    }
    if (!Inventory || !GetActiveRecipe().IsValid() || !GetWorld())
    {
        SetState(EProductionState::Stopped);
        return;
    }

    SetState(EProductionState::WaitingForInputs);
    GetWorld()->GetTimerManager().SetTimer(
        SimulationTimer, this, &UProductionComponent::SimulateProduction, SimulationInterval, true, 0.0f);
}

void UProductionComponent::StopProduction()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(SimulationTimer);
    }
    RemainingSeconds = 0.0f;
    SetState(EProductionState::Stopped);
}

void UProductionComponent::SetInlineRecipe(const FProductionRecipeSpec& NewRecipe)
{
    InlineRecipe = NewRecipe;
}

FProductionRecipeSpec UProductionComponent::GetActiveRecipe() const
{
    return RecipeDefinition ? RecipeDefinition->Recipe : InlineRecipe;
}

void UProductionComponent::SimulateProduction()
{
    if (!Inventory)
    {
        StopProduction();
        return;
    }

    const FProductionRecipeSpec Recipe = GetActiveRecipe();
    if (!Recipe.IsValid())
    {
        StopProduction();
        return;
    }

    if (State == EProductionState::Producing)
    {
        RemainingSeconds = FMath::Max(0.0f, RemainingSeconds - SimulationInterval);
        if (RemainingSeconds <= 0.0f)
        {
            if (Inventory->CanAddResources(Recipe.Outputs))
            {
                Inventory->AddResources(Recipe.Outputs);
                OnProductionBatchCompleted.Broadcast(Recipe.RecipeId);
                SetState(EProductionState::WaitingForInputs);
            }
            else
            {
                SetState(EProductionState::OutputBlocked);
            }
        }
        return;
    }

    if (State == EProductionState::OutputBlocked)
    {
        if (Inventory->CanAddResources(Recipe.Outputs))
        {
            Inventory->AddResources(Recipe.Outputs);
            OnProductionBatchCompleted.Broadcast(Recipe.RecipeId);
            SetState(EProductionState::WaitingForInputs);
        }
        return;
    }

    if (Inventory->HasResources(Recipe.Inputs) && HasRoomAfterConsumingInputs(Recipe))
    {
        Inventory->RemoveResources(Recipe.Inputs);
        RemainingSeconds = Recipe.DurationSeconds;
        SetState(EProductionState::Producing);
    }
    else
    {
        SetState(EProductionState::WaitingForInputs);
    }
}

void UProductionComponent::SetState(const EProductionState NewState)
{
    if (State == NewState)
    {
        return;
    }
    const EProductionState OldState = State;
    State = NewState;
    OnProductionStateChanged.Broadcast(OldState, State);
}

bool UProductionComponent::HasRoomAfterConsumingInputs(const FProductionRecipeSpec& Recipe) const
{
    if (!Inventory || Inventory->GetCapacity() < 0)
    {
        return true;
    }

    int32 InputUnits = 0;
    int32 OutputUnits = 0;
    for (const FResourceAmount& Input : Recipe.Inputs)
    {
        InputUnits += Input.Amount;
    }
    for (const FResourceAmount& Output : Recipe.Outputs)
    {
        OutputUnits += Output.Amount;
    }
    return Inventory->GetTotalUnits() - InputUnits + OutputUnits <= Inventory->GetCapacity();
}
