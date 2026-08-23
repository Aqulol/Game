#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Resources/ResourceTypes.h"
#include "BuildingDefinition.generated.h"

class ABuildingBase;

/** Optional data-driven building entry. Native prototype classes remain usable without assets. */
UCLASS(BlueprintType)
class SETTLEMENTGAME_API UBuildingDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    virtual FPrimaryAssetId GetPrimaryAssetId() const override
    {
        return FPrimaryAssetId(TEXT("Building"), BuildingId);
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
    FName BuildingId = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
    TSubclassOf<ABuildingBase> BuildingClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
    TArray<FResourceAmount> BuildCost;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
    FVector Footprint = FVector(300.0f, 300.0f, 200.0f);
};
