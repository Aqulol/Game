#include "Workers/SettlementWorkerCharacter.h"

#include "AIController.h"
#include "Buildings/PrototypeBuildings.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Data/WorkerJobDefinition.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Inventory/ResourceInventoryComponent.h"
#include "Resources/HarvestableResourceNode.h"
#include "Resources/ResourceTypes.h"
#include "Time/SettlementTimeSubsystem.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

ASettlementWorkerCharacter::ASettlementWorkerCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    WorkerName = NSLOCTEXT("Settlement", "DefaultWorkerName", "Aldric");

    GetCharacterMovement()->MaxWalkSpeed = 300.0f;
    CarriedInventory = CreateDefaultSubobject<UResourceInventoryComponent>(TEXT("CarriedInventory"));
    CarriedInventory->SetCapacity(10);

    PlaceholderBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderBody"));
    PlaceholderBody->SetupAttachment(GetCapsuleComponent());
    PlaceholderBody->SetRelativeLocation(FVector(0.0f, 0.0f, -5.0f));
    PlaceholderBody->SetRelativeScale3D(FVector(0.55f, 0.45f, 1.65f));
    PlaceholderBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        PlaceholderBody->SetStaticMesh(CubeMesh.Object);
    }
}

void ASettlementWorkerCharacter::BeginPlay()
{
    Super::BeginPlay();
    SpawnDefaultController();
    GetWorld()->GetTimerManager().SetTimer(
        SimulationTimer, this, &ASettlementWorkerCharacter::SimulateWorker, SimulationInterval, true, SimulationInterval);
}

void ASettlementWorkerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(SimulationTimer);
    }
    Super::EndPlay(EndPlayReason);
}

bool ASettlementWorkerCharacter::CanInteract_Implementation(AActor* Interactor) const
{
    return IsValid(Interactor);
}

FText ASettlementWorkerCharacter::GetInteractionPrompt_Implementation(AActor* Interactor) const
{
    return FText::FromString(FString::Printf(
        TEXT("%s - assign/cycle job (now: %s)"), *WorkerName.ToString(), *GetJobText().ToString()));
}

void ASettlementWorkerCharacter::Interact_Implementation(AActor* Interactor)
{
    switch (Job)
    {
    case EWorkerJobBehavior::None:
        AssignJob(EWorkerJobBehavior::Lumberjack);
        break;
    case EWorkerJobBehavior::Lumberjack:
        if (!AssignJob(EWorkerJobBehavior::Sawyer, FindNearestSawmill()))
        {
            AssignJob(EWorkerJobBehavior::None);
        }
        break;
    case EWorkerJobBehavior::Sawyer:
    default:
        AssignJob(EWorkerJobBehavior::None);
        break;
    }
}

bool ASettlementWorkerCharacter::AssignJob(const EWorkerJobBehavior NewJob, ABuildingBase* NewWorkplace)
{
    if (NewJob == EWorkerJobBehavior::Sawyer)
    {
        ASawmillBuilding* Sawmill = Cast<ASawmillBuilding>(NewWorkplace);
        if (!Sawmill)
        {
            Sawmill = FindNearestSawmill();
        }
        if (!Sawmill)
        {
            return false;
        }
        Workplace = Sawmill;
    }
    else
    {
        Workplace = NewWorkplace;
    }

    Job = NewJob;
    if (AAIController* AI = Cast<AAIController>(GetController()))
    {
        AI->StopMovement();
    }
    SetTask(EWorkerTaskState::Idle);
    return true;
}

bool ASettlementWorkerCharacter::AssignJobDefinition(UWorkerJobDefinition* JobDefinition, ABuildingBase* NewWorkplace)
{
    return JobDefinition && AssignJob(JobDefinition->Behavior, NewWorkplace);
}

FText ASettlementWorkerCharacter::GetCurrentTaskText() const
{
    switch (TaskState)
    {
    case EWorkerTaskState::Idle: return NSLOCTEXT("Settlement", "TaskIdle", "Idle / finding task");
    case EWorkerTaskState::MovingToResource: return NSLOCTEXT("Settlement", "TaskMoveResource", "Walking to a tree");
    case EWorkerTaskState::Harvesting: return NSLOCTEXT("Settlement", "TaskHarvest", "Gathering wood");
    case EWorkerTaskState::MovingToStorageToDeposit: return NSLOCTEXT("Settlement", "TaskDeposit", "Delivering resources");
    case EWorkerTaskState::MovingToStorageForInput: return NSLOCTEXT("Settlement", "TaskInput", "Collecting sawmill input");
    case EWorkerTaskState::MovingToWorkplace: return NSLOCTEXT("Settlement", "TaskWorkplace", "Supplying the sawmill");
    case EWorkerTaskState::WaitingForProduction: return NSLOCTEXT("Settlement", "TaskProduction", "Working at the sawmill");
    case EWorkerTaskState::MovingHome: return NSLOCTEXT("Settlement", "TaskHome", "Going home");
    case EWorkerTaskState::Resting: return NSLOCTEXT("Settlement", "TaskRest", "Resting");
    default: return FText::GetEmpty();
    }
}

FText ASettlementWorkerCharacter::GetJobText() const
{
    switch (Job)
    {
    case EWorkerJobBehavior::Lumberjack: return NSLOCTEXT("Settlement", "JobLumberjack", "Lumberjack");
    case EWorkerJobBehavior::Sawyer: return NSLOCTEXT("Settlement", "JobSawyer", "Sawyer");
    default: return NSLOCTEXT("Settlement", "JobNone", "Unassigned");
    }
}

void ASettlementWorkerCharacter::SimulateWorker()
{
    Hunger = FMath::Clamp(Hunger + 0.25f * SimulationInterval, 0.0f, 100.0f);

    const USettlementTimeSubsystem* Time = GetWorld()->GetSubsystem<USettlementTimeSubsystem>();
    const bool bWorkPhase = !Time || Time->GetCurrentPhase() == ESettlementDayPhase::Work;
    if (!bWorkPhase || Energy <= 15.0f)
    {
        BeginRest();
    }

    if (TaskState == EWorkerTaskState::MovingHome)
    {
        if (HasReachedTarget() || !TaskTarget)
        {
            SetTask(EWorkerTaskState::Resting);
        }
        return;
    }

    if (TaskState == EWorkerTaskState::Resting)
    {
        Energy = FMath::Clamp(Energy + 6.0f * SimulationInterval, 0.0f, 100.0f);
        if (bWorkPhase && Energy >= 80.0f)
        {
            SetTask(EWorkerTaskState::Idle);
        }
        return;
    }

    Energy = FMath::Clamp(Energy - 0.20f * SimulationInterval, 0.0f, 100.0f);
    if (Job == EWorkerJobBehavior::Lumberjack)
    {
        SimulateLumberjack();
    }
    else if (Job == EWorkerJobBehavior::Sawyer)
    {
        SimulateSawyer();
    }
    else
    {
        SetTask(EWorkerTaskState::Idle);
    }
}

void ASettlementWorkerCharacter::SimulateLumberjack()
{
    switch (TaskState)
    {
    case EWorkerTaskState::Idle:
        if (CarriedInventory->GetTotalUnits() > 0)
        {
            MoveToTarget(FindNearestStorage(), EWorkerTaskState::MovingToStorageToDeposit);
        }
        else
        {
            MoveToTarget(FindNearestWoodNode(), EWorkerTaskState::MovingToResource);
        }
        break;

    case EWorkerTaskState::MovingToResource:
        if (!TaskTarget)
        {
            SetTask(EWorkerTaskState::Idle);
        }
        else if (HasReachedTarget())
        {
            if (AAIController* AI = Cast<AAIController>(GetController()))
            {
                AI->StopMovement();
            }
            TaskTimeRemaining = HarvestDurationSeconds;
            SetTask(EWorkerTaskState::Harvesting, TaskTarget);
        }
        break;

    case EWorkerTaskState::Harvesting:
        TaskTimeRemaining -= SimulationInterval;
        if (TaskTimeRemaining <= 0.0f)
        {
            AHarvestableResourceNode* Node = Cast<AHarvestableResourceNode>(TaskTarget);
            const int32 Gathered = Node ? Node->Harvest(HarvestUnitsPerTrip) : 0;
            CarriedInventory->AddResource(FSettlementResourceIds::Wood, Gathered);
            if (!MoveToTarget(FindNearestStorage(), EWorkerTaskState::MovingToStorageToDeposit))
            {
                SetTask(EWorkerTaskState::Idle);
            }
        }
        break;

    case EWorkerTaskState::MovingToStorageToDeposit:
        if (!TaskTarget)
        {
            SetTask(EWorkerTaskState::Idle);
        }
        else if (HasReachedTarget())
        {
            if (AStorageBuilding* Storage = Cast<AStorageBuilding>(TaskTarget))
            {
                CarriedInventory->TransferAllTo(Storage->GetInventory());
            }
            SetTask(EWorkerTaskState::Idle);
        }
        break;

    default:
        SetTask(EWorkerTaskState::Idle);
        break;
    }
}

void ASettlementWorkerCharacter::SimulateSawyer()
{
    ASawmillBuilding* Sawmill = Cast<ASawmillBuilding>(Workplace);
    if (!Sawmill)
    {
        AssignJob(EWorkerJobBehavior::None);
        return;
    }

    switch (TaskState)
    {
    case EWorkerTaskState::Idle:
        if (CarriedInventory->GetResourceAmount(FSettlementResourceIds::Planks) > 0)
        {
            MoveToTarget(FindNearestStorage(), EWorkerTaskState::MovingToStorageToDeposit);
        }
        else if (CarriedInventory->GetResourceAmount(FSettlementResourceIds::Wood) >= 2)
        {
            MoveToTarget(Sawmill, EWorkerTaskState::MovingToWorkplace);
        }
        else
        {
            MoveToTarget(FindNearestStorage(FSettlementResourceIds::Wood, 2), EWorkerTaskState::MovingToStorageForInput);
        }
        break;

    case EWorkerTaskState::MovingToStorageForInput:
        if (!TaskTarget)
        {
            SetTask(EWorkerTaskState::Idle);
        }
        else if (HasReachedTarget())
        {
            AStorageBuilding* Storage = Cast<AStorageBuilding>(TaskTarget);
            const int32 Moved = Storage
                ? Storage->GetInventory()->TransferResourceTo(CarriedInventory, FSettlementResourceIds::Wood, 2)
                : 0;
            if (Moved >= 2)
            {
                MoveToTarget(Sawmill, EWorkerTaskState::MovingToWorkplace);
            }
            else
            {
                SetTask(EWorkerTaskState::Idle);
            }
        }
        break;

    case EWorkerTaskState::MovingToWorkplace:
        if (!TaskTarget)
        {
            SetTask(EWorkerTaskState::Idle);
        }
        else if (HasReachedTarget())
        {
            CarriedInventory->TransferResourceTo(Sawmill->GetInventory(), FSettlementResourceIds::Wood, 2);
            SetTask(EWorkerTaskState::WaitingForProduction, Sawmill);
        }
        break;

    case EWorkerTaskState::WaitingForProduction:
        if (Sawmill->GetInventory()->GetResourceAmount(FSettlementResourceIds::Planks) >= 2)
        {
            Sawmill->GetInventory()->TransferResourceTo(CarriedInventory, FSettlementResourceIds::Planks, 2);
            MoveToTarget(FindNearestStorage(), EWorkerTaskState::MovingToStorageToDeposit);
        }
        break;

    case EWorkerTaskState::MovingToStorageToDeposit:
        if (!TaskTarget)
        {
            SetTask(EWorkerTaskState::Idle);
        }
        else if (HasReachedTarget())
        {
            if (AStorageBuilding* Storage = Cast<AStorageBuilding>(TaskTarget))
            {
                CarriedInventory->TransferAllTo(Storage->GetInventory());
            }
            SetTask(EWorkerTaskState::Idle);
        }
        break;

    default:
        SetTask(EWorkerTaskState::Idle);
        break;
    }
}

void ASettlementWorkerCharacter::BeginRest()
{
    if (TaskState == EWorkerTaskState::MovingHome || TaskState == EWorkerTaskState::Resting)
    {
        return;
    }
    if (Home && MoveToTarget(Home, EWorkerTaskState::MovingHome))
    {
        return;
    }
    SetTask(EWorkerTaskState::Resting);
}

void ASettlementWorkerCharacter::SetTask(const EWorkerTaskState NewState, AActor* NewTarget)
{
    TaskState = NewState;
    TaskTarget = NewTarget;
}

bool ASettlementWorkerCharacter::MoveToTarget(AActor* NewTarget, const EWorkerTaskState MovingState)
{
    if (!IsValid(NewTarget))
    {
        return false;
    }
    if (!GetController())
    {
        SpawnDefaultController();
    }
    AAIController* AI = Cast<AAIController>(GetController());
    if (!AI)
    {
        return false;
    }

    SetTask(MovingState, NewTarget);
    AI->MoveToActor(NewTarget, AcceptanceRadius, true, true, true, nullptr, true);
    return true;
}

bool ASettlementWorkerCharacter::HasReachedTarget() const
{
    if (!IsValid(TaskTarget))
    {
        return false;
    }

    const FBox Bounds = TaskTarget->GetComponentsBoundingBox(true);
    const FVector ClosestPoint = Bounds.IsValid ? Bounds.GetClosestPointTo(GetActorLocation()) : TaskTarget->GetActorLocation();
    return FVector::DistSquared2D(GetActorLocation(), ClosestPoint) <= FMath::Square(AcceptanceRadius + 50.0f);
}

AHarvestableResourceNode* ASettlementWorkerCharacter::FindNearestWoodNode() const
{
    AHarvestableResourceNode* Best = nullptr;
    double BestDistanceSq = TNumericLimits<double>::Max();
    for (TActorIterator<AHarvestableResourceNode> It(GetWorld()); It; ++It)
    {
        if (It->GetResourceId() != FSettlementResourceIds::Wood || It->GetRemainingAmount() <= 0)
        {
            continue;
        }
        const double DistanceSq = FVector::DistSquared(GetActorLocation(), It->GetActorLocation());
        if (DistanceSq < BestDistanceSq)
        {
            BestDistanceSq = DistanceSq;
            Best = *It;
        }
    }
    return Best;
}

AStorageBuilding* ASettlementWorkerCharacter::FindNearestStorage(const FName RequiredResource, const int32 RequiredAmount) const
{
    AStorageBuilding* Best = nullptr;
    double BestDistanceSq = TNumericLimits<double>::Max();
    for (TActorIterator<AStorageBuilding> It(GetWorld()); It; ++It)
    {
        if (It->IsPreview() || (RequiredAmount > 0 && !It->GetInventory()->HasResource(RequiredResource, RequiredAmount)))
        {
            continue;
        }
        const double DistanceSq = FVector::DistSquared(GetActorLocation(), It->GetActorLocation());
        if (DistanceSq < BestDistanceSq)
        {
            BestDistanceSq = DistanceSq;
            Best = *It;
        }
    }
    return Best;
}

ASawmillBuilding* ASettlementWorkerCharacter::FindNearestSawmill() const
{
    ASawmillBuilding* Best = nullptr;
    double BestDistanceSq = TNumericLimits<double>::Max();
    for (TActorIterator<ASawmillBuilding> It(GetWorld()); It; ++It)
    {
        if (It->IsPreview())
        {
            continue;
        }
        const double DistanceSq = FVector::DistSquared(GetActorLocation(), It->GetActorLocation());
        if (DistanceSq < BestDistanceSq)
        {
            BestDistanceSq = DistanceSq;
            Best = *It;
        }
    }
    return Best;
}
