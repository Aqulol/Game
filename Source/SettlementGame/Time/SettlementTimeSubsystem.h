#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SettlementTimeSubsystem.generated.h"

UENUM(BlueprintType)
enum class ESettlementDayPhase : uint8
{
    Work,
    Rest,
    Sleep
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDayPhaseChanged, ESettlementDayPhase, OldPhase, ESettlementDayPhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSettlementClockUpdated, int32, Day, float, Hour);

/** One centralized world timer drives game time; simulation actors do not need per-frame Tick. */
UCLASS()
class SETTLEMENTGAME_API USettlementTimeSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintPure, Category = "Time")
    int32 GetDay() const;

    UFUNCTION(BlueprintPure, Category = "Time")
    float GetHour() const;

    UFUNCTION(BlueprintPure, Category = "Time")
    ESettlementDayPhase GetCurrentPhase() const { return CurrentPhase; }

    UFUNCTION(BlueprintCallable, Category = "Time")
    void SetPaused(bool bNewPaused) { bPaused = bNewPaused; }

    UFUNCTION(BlueprintCallable, Category = "Time")
    void SetTimeScale(float NewTimeScale) { TimeScale = FMath::Max(0.0f, NewTimeScale); }

    UPROPERTY(BlueprintAssignable, Category = "Time")
    FOnDayPhaseChanged OnDayPhaseChanged;

    UPROPERTY(BlueprintAssignable, Category = "Time")
    FOnSettlementClockUpdated OnClockUpdated;

private:
    UPROPERTY()
    float DayLengthRealSeconds = 240.0f;

    UPROPERTY()
    float TimeScale = 1.0f;

    UPROPERTY()
    bool bPaused = false;

    double AbsoluteGameMinutes = 8.0 * 60.0;
    ESettlementDayPhase CurrentPhase = ESettlementDayPhase::Work;
    FTimerHandle ClockTimer;
    float ClockInterval = 0.25f;

    void AdvanceClock();
    ESettlementDayPhase CalculatePhase(float Hour) const;
};
