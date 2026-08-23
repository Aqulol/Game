#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Workers/WorkerTypes.h"
#include "WorkerJobDefinition.generated.h"

/** Jobs are data-driven; Behavior selects the small native strategy used by this vertical slice. */
UCLASS(BlueprintType)
class SETTLEMENTGAME_API UWorkerJobDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    virtual FPrimaryAssetId GetPrimaryAssetId() const override
    {
        return FPrimaryAssetId(TEXT("Job"), JobId);
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Job")
    FName JobId = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Job")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Job")
    EWorkerJobBehavior Behavior = EWorkerJobBehavior::None;
};
