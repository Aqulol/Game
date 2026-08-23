#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ResourceDefinition.generated.h"

class UTexture2D;

/** Designer-authored metadata. Runtime state stores only ResourceId, never an asset pointer. */
UCLASS(BlueprintType)
class SETTLEMENTGAME_API UResourceDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    virtual FPrimaryAssetId GetPrimaryAssetId() const override
    {
        return FPrimaryAssetId(TEXT("Resource"), ResourceId);
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource")
    FName ResourceId = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource")
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource", meta = (ClampMin = "1"))
    int32 MaxStackSize = 100;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource", meta = (ClampMin = "0.0"))
    float UnitWeight = 1.0f;
};
