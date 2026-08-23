#include "Inventory/ResourceInventoryComponent.h"

UResourceInventoryComponent::UResourceInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

int32 UResourceInventoryComponent::GetResourceAmount(const FName ResourceId) const
{
    return Resources.FindRef(ResourceId);
}

int32 UResourceInventoryComponent::GetTotalUnits() const
{
    int32 Total = 0;
    for (const TPair<FName, int32>& Entry : Resources)
    {
        Total += FMath::Max(0, Entry.Value);
    }
    return Total;
}

int32 UResourceInventoryComponent::GetFreeCapacity() const
{
    return Capacity < 0 ? MAX_int32 : FMath::Max(0, Capacity - GetTotalUnits());
}

bool UResourceInventoryComponent::CanAddResource(const FName ResourceId, const int32 Amount) const
{
    return !ResourceId.IsNone() && Amount >= 0 && (Capacity < 0 || GetTotalUnits() + Amount <= Capacity);
}

bool UResourceInventoryComponent::HasResource(const FName ResourceId, const int32 Amount) const
{
    return Amount >= 0 && GetResourceAmount(ResourceId) >= Amount;
}

bool UResourceInventoryComponent::HasResources(const TArray<FResourceAmount>& RequiredResources) const
{
    TMap<FName, int32> Totals;
    for (const FResourceAmount& Entry : RequiredResources)
    {
        if (Entry.Amount < 0 || Entry.ResourceId.IsNone())
        {
            return false;
        }
        Totals.FindOrAdd(Entry.ResourceId) += Entry.Amount;
    }

    for (const TPair<FName, int32>& Entry : Totals)
    {
        if (!HasResource(Entry.Key, Entry.Value))
        {
            return false;
        }
    }
    return true;
}

bool UResourceInventoryComponent::CanAddResources(const TArray<FResourceAmount>& ResourcesToAdd) const
{
    int32 AddedUnits = 0;
    for (const FResourceAmount& Entry : ResourcesToAdd)
    {
        if (Entry.Amount < 0 || Entry.ResourceId.IsNone())
        {
            return false;
        }
        AddedUnits += Entry.Amount;
    }
    return Capacity < 0 || GetTotalUnits() + AddedUnits <= Capacity;
}

int32 UResourceInventoryComponent::AddResource(const FName ResourceId, const int32 Amount)
{
    if (ResourceId.IsNone() || Amount <= 0)
    {
        return 0;
    }

    const int32 Accepted = Capacity < 0 ? Amount : FMath::Min(Amount, GetFreeCapacity());
    if (Accepted <= 0)
    {
        return 0;
    }

    int32& Current = Resources.FindOrAdd(ResourceId);
    Current += Accepted;
    OnResourceAmountChanged.Broadcast(ResourceId, Current, Accepted);
    return Accepted;
}

int32 UResourceInventoryComponent::RemoveResource(const FName ResourceId, const int32 Amount)
{
    if (ResourceId.IsNone() || Amount <= 0)
    {
        return 0;
    }

    int32* Current = Resources.Find(ResourceId);
    if (!Current)
    {
        return 0;
    }

    const int32 Removed = FMath::Min(Amount, *Current);
    *Current -= Removed;
    const int32 NewAmount = *Current;
    if (*Current <= 0)
    {
        Resources.Remove(ResourceId);
    }
    OnResourceAmountChanged.Broadcast(ResourceId, NewAmount, -Removed);
    return Removed;
}

bool UResourceInventoryComponent::RemoveResources(const TArray<FResourceAmount>& RequiredResources)
{
    if (!HasResources(RequiredResources))
    {
        return false;
    }
    for (const FResourceAmount& Entry : RequiredResources)
    {
        RemoveResource(Entry.ResourceId, Entry.Amount);
    }
    return true;
}

bool UResourceInventoryComponent::AddResources(const TArray<FResourceAmount>& ResourcesToAdd)
{
    if (!CanAddResources(ResourcesToAdd))
    {
        return false;
    }
    for (const FResourceAmount& Entry : ResourcesToAdd)
    {
        AddResource(Entry.ResourceId, Entry.Amount);
    }
    return true;
}

int32 UResourceInventoryComponent::TransferResourceTo(UResourceInventoryComponent* Target, const FName ResourceId, const int32 Amount)
{
    if (!IsValid(Target) || Target == this || Amount <= 0)
    {
        return 0;
    }

    const int32 Transferable = FMath::Min3(Amount, GetResourceAmount(ResourceId), Target->GetFreeCapacity());
    if (Transferable <= 0)
    {
        return 0;
    }

    const int32 Removed = RemoveResource(ResourceId, Transferable);
    const int32 Added = Target->AddResource(ResourceId, Removed);
    if (Added < Removed)
    {
        AddResource(ResourceId, Removed - Added);
    }
    return Added;
}

int32 UResourceInventoryComponent::TransferAllTo(UResourceInventoryComponent* Target)
{
    if (!IsValid(Target) || Target == this)
    {
        return 0;
    }

    int32 Transferred = 0;
    const TArray<FResourceAmount> Snapshot = GetContents();
    for (const FResourceAmount& Entry : Snapshot)
    {
        Transferred += TransferResourceTo(Target, Entry.ResourceId, Entry.Amount);
    }
    return Transferred;
}

TArray<FResourceAmount> UResourceInventoryComponent::GetContents() const
{
    TArray<FResourceAmount> Result;
    Result.Reserve(Resources.Num());
    for (const TPair<FName, int32>& Entry : Resources)
    {
        if (Entry.Value > 0)
        {
            Result.Emplace(Entry.Key, Entry.Value);
        }
    }
    Result.Sort([](const FResourceAmount& A, const FResourceAmount& B)
    {
        return A.ResourceId.LexicalLess(B.ResourceId);
    });
    return Result;
}

FString UResourceInventoryComponent::ToDebugString() const
{
    const TArray<FResourceAmount> Contents = GetContents();
    if (Contents.IsEmpty())
    {
        return TEXT("empty");
    }

    TArray<FString> Parts;
    Parts.Reserve(Contents.Num());
    for (const FResourceAmount& Entry : Contents)
    {
        Parts.Add(FString::Printf(TEXT("%s: %d"), *Entry.ResourceId.ToString(), Entry.Amount));
    }
    return FString::Join(Parts, TEXT(" | "));
}

void UResourceInventoryComponent::SetCapacity(const int32 NewCapacity)
{
    Capacity = NewCapacity;
}
