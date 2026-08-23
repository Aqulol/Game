#pragma once

#include "CoreMinimal.h"
#include "Resources/ResourceTypes.h"
#include "ProductionTypes.generated.h"

USTRUCT(BlueprintType)
struct SETTLEMENTGAME_API FProductionRecipeSpec
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Production")
    FName RecipeId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Production")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Production")
    TArray<FResourceAmount> Inputs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Production")
    TArray<FResourceAmount> Outputs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Production", meta = (ClampMin = "0.1"))
    float DurationSeconds = 4.0f;

    bool IsValid() const { return !RecipeId.IsNone() && !Outputs.IsEmpty() && DurationSeconds > 0.0f; }
};

UENUM(BlueprintType)
enum class EProductionState : uint8
{
    Stopped,
    WaitingForInputs,
    Producing,
    OutputBlocked
};
