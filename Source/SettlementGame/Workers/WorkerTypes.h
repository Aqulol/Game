#pragma once

#include "CoreMinimal.h"
#include "WorkerTypes.generated.h"

UENUM(BlueprintType)
enum class EWorkerJobBehavior : uint8
{
    None,
    Lumberjack,
    Sawyer
};

UENUM(BlueprintType)
enum class EWorkerTaskState : uint8
{
    Idle,
    MovingToResource,
    Harvesting,
    MovingToStorageToDeposit,
    MovingToStorageForInput,
    MovingToWorkplace,
    WaitingForProduction,
    MovingHome,
    Resting
};
