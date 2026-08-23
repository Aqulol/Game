#pragma once

#include "CoreMinimal.h"
#include "ResourceTypes.generated.h"

/** A serializable resource/quantity pair used by inventories, costs and recipes. */
USTRUCT(BlueprintType)
struct SETTLEMENTGAME_API FResourceAmount
{
    GENERATED_BODY()

    FResourceAmount() = default;
    FResourceAmount(const FName InResourceId, const int32 InAmount)
        : ResourceId(InResourceId), Amount(InAmount)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
    FName ResourceId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource", meta = (ClampMin = "0"))
    int32 Amount = 0;

    bool IsValid() const { return !ResourceId.IsNone() && Amount > 0; }
};

/** Stable IDs for the native vertical-slice content. Data Assets may add more IDs. */
struct SETTLEMENTGAME_API FSettlementResourceIds
{
    static const FName Wood;
    static const FName Stone;
    static const FName Planks;
};
