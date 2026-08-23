#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PrototypeGameMode.generated.h"

/** Spawns an asset-free test clearing, resource nodes and runtime navigation into an empty level. */
UCLASS()
class SETTLEMENTGAME_API APrototypeGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    APrototypeGameMode();
    virtual void BeginPlay() override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Prototype")
    bool bSpawnPrototypeSandbox = true;

private:
    void SpawnSandbox();
};
