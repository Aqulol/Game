#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SettlementHUD.generated.h"

/** Canvas HUD keeps the repository immediately playable; replace with UMG without changing gameplay code. */
UCLASS()
class SETTLEMENTGAME_API ASettlementHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

private:
    FString FormatResourceLine(class UResourceInventoryComponent* Inventory) const;
};
