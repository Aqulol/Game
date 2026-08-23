#include "Time/SettlementTimeSubsystem.h"

#include "Engine/World.h"
#include "TimerManager.h"

void USettlementTimeSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    CurrentPhase = CalculatePhase(GetHour());
    InWorld.GetTimerManager().SetTimer(ClockTimer, this, &USettlementTimeSubsystem::AdvanceClock, ClockInterval, true);
}

void USettlementTimeSubsystem::Deinitialize()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(ClockTimer);
    }
    Super::Deinitialize();
}

int32 USettlementTimeSubsystem::GetDay() const
{
    return 1 + FMath::FloorToInt(AbsoluteGameMinutes / 1440.0);
}

float USettlementTimeSubsystem::GetHour() const
{
    return static_cast<float>(FMath::Fmod(AbsoluteGameMinutes, 1440.0) / 60.0);
}

void USettlementTimeSubsystem::AdvanceClock()
{
    if (bPaused || DayLengthRealSeconds <= 0.0f)
    {
        return;
    }

    AbsoluteGameMinutes += static_cast<double>(ClockInterval * TimeScale * 1440.0f / DayLengthRealSeconds);
    const ESettlementDayPhase NewPhase = CalculatePhase(GetHour());
    if (NewPhase != CurrentPhase)
    {
        const ESettlementDayPhase OldPhase = CurrentPhase;
        CurrentPhase = NewPhase;
        OnDayPhaseChanged.Broadcast(OldPhase, NewPhase);
    }
    OnClockUpdated.Broadcast(GetDay(), GetHour());
}

ESettlementDayPhase USettlementTimeSubsystem::CalculatePhase(const float Hour) const
{
    if (Hour >= 6.0f && Hour < 18.0f)
    {
        return ESettlementDayPhase::Work;
    }
    if (Hour >= 18.0f && Hour < 22.0f)
    {
        return ESettlementDayPhase::Rest;
    }
    return ESettlementDayPhase::Sleep;
}
