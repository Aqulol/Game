#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Production/ProductionTypes.h"
#include "ProductionRecipeDefinition.generated.h"

UCLASS(BlueprintType)
class SETTLEMENTGAME_API UProductionRecipeDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    virtual FPrimaryAssetId GetPrimaryAssetId() const override
    {
        return FPrimaryAssetId(TEXT("Recipe"), Recipe.RecipeId);
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Production")
    FProductionRecipeSpec Recipe;
};
