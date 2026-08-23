#include "UI/SettlementHUD.h"

#include "Buildings/BuildingPlacementComponent.h"
#include "Buildings/PrototypeBuildings.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "Interaction/Interactable.h"
#include "Interaction/InteractionComponent.h"
#include "Inventory/ResourceInventoryComponent.h"
#include "Player/SettlementPlayerCharacter.h"
#include "Resources/ResourceTypes.h"
#include "Time/SettlementTimeSubsystem.h"
#include "Workers/SettlementWorkerCharacter.h"

void ASettlementHUD::DrawHUD()
{
    Super::DrawHUD();
    if (!Canvas)
    {
        return;
    }

    ASettlementPlayerCharacter* Player = Cast<ASettlementPlayerCharacter>(GetOwningPawn());
    if (!Player)
    {
        return;
    }

    const UFont* Font = GEngine ? GEngine->GetSmallFont() : nullptr;
    float Y = 30.0f;
    const auto DrawLine = [this, Font, &Y](const FString& Text, const FLinearColor Color = FLinearColor::White)
    {
        DrawText(Text, Color, 30.0f, Y, const_cast<UFont*>(Font), 1.05f, false);
        Y += 22.0f;
    };

    DrawLine(TEXT("SETTLEMENT PROTOTYPE"), FLinearColor(0.95f, 0.78f, 0.35f));
    DrawLine(FString::Printf(TEXT("Carried: %s"), *FormatResourceLine(Player->GetInventory())));

    AStorageBuilding* Storage = nullptr;
    for (TActorIterator<AStorageBuilding> It(GetWorld()); It; ++It)
    {
        if (!It->IsPreview())
        {
            Storage = *It;
            break;
        }
    }
    DrawLine(FString::Printf(TEXT("Warehouse: %s"), Storage ? *FormatResourceLine(Storage->GetInventory()) : TEXT("not built")));

    if (const USettlementTimeSubsystem* Time = GetWorld()->GetSubsystem<USettlementTimeSubsystem>())
    {
        DrawLine(FString::Printf(TEXT("Day %d, %02d:%02d"), Time->GetDay(), FMath::FloorToInt(Time->GetHour()),
            FMath::FloorToInt(FMath::Frac(Time->GetHour()) * 60.0f)));
    }

    for (TActorIterator<ASettlementWorkerCharacter> It(GetWorld()); It; ++It)
    {
        DrawLine(FString::Printf(TEXT("Resident %s | %s | energy %.0f | hunger %.0f"),
            *It->GetWorkerName().ToString(), *It->GetJobText().ToString(), It->GetEnergy(), It->GetHunger()));
        DrawLine(FString::Printf(TEXT("Task: %s"), *It->GetCurrentTaskText().ToString()));
        break;
    }

    Y += 8.0f;
    DrawLine(TEXT("E gather/interact | 1 warehouse | 2 house | 3 sawmill | 4 plank platform"), FLinearColor(0.75f, 0.85f, 1.0f));
    DrawLine(TEXT("LMB place | RMB cancel | interact with resident to cycle job"), FLinearColor(0.75f, 0.85f, 1.0f));

    const UBuildingPlacementComponent* Placement = Player->GetBuildingPlacementComponent();
    if (Placement && Placement->IsPlacementActive())
    {
        DrawLine(Placement->IsCurrentLocationValid() ? TEXT("BUILD MODE: valid location") : TEXT("BUILD MODE: blocked/invalid"),
            Placement->IsCurrentLocationValid() ? FLinearColor::Green : FLinearColor::Red);
    }

    const UInteractionComponent* Interaction = Player->GetInteractionComponent();
    AActor* Focused = Interaction ? Interaction->GetFocusedActor() : nullptr;
    if (Focused && Focused->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
    {
        const FText Prompt = IInteractable::Execute_GetInteractionPrompt(Focused, Player);
        const float CenterX = Canvas->ClipX * 0.5f;
        const float CenterY = Canvas->ClipY * 0.5f + 35.0f;
        DrawText(FString::Printf(TEXT("[E] %s"), *Prompt.ToString()), FLinearColor::White, CenterX - 130.0f, CenterY,
            const_cast<UFont*>(Font), 1.1f, false);
    }

    const float CrossX = Canvas->ClipX * 0.5f;
    const float CrossY = Canvas->ClipY * 0.5f;
    DrawRect(FLinearColor::White, CrossX - 1.0f, CrossY - 7.0f, 2.0f, 14.0f);
    DrawRect(FLinearColor::White, CrossX - 7.0f, CrossY - 1.0f, 14.0f, 2.0f);
}

FString ASettlementHUD::FormatResourceLine(UResourceInventoryComponent* Inventory) const
{
    if (!Inventory)
    {
        return TEXT("-");
    }
    return FString::Printf(TEXT("wood %d | stone %d | planks %d"),
        Inventory->GetResourceAmount(FSettlementResourceIds::Wood),
        Inventory->GetResourceAmount(FSettlementResourceIds::Stone),
        Inventory->GetResourceAmount(FSettlementResourceIds::Planks));
}
