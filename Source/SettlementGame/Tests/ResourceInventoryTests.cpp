#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Inventory/ResourceInventoryComponent.h"
#include "Resources/ResourceTypes.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FSettlementInventoryCapacityTest,
    "Settlement.Inventory.CapacityAndTransfer",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSettlementInventoryCapacityTest::RunTest(const FString& Parameters)
{
    UResourceInventoryComponent* Source = NewObject<UResourceInventoryComponent>();
    UResourceInventoryComponent* Target = NewObject<UResourceInventoryComponent>();
    Source->SetCapacity(10);
    Target->SetCapacity(3);

    TestEqual(TEXT("Source accepts its full capacity"), Source->AddResource(FSettlementResourceIds::Wood, 12), 10);
    TestEqual(TEXT("Target accepts only three transferred units"),
        Source->TransferResourceTo(Target, FSettlementResourceIds::Wood, 8), 3);
    TestEqual(TEXT("Source retains seven units"), Source->GetResourceAmount(FSettlementResourceIds::Wood), 7);
    TestEqual(TEXT("Target has three units"), Target->GetResourceAmount(FSettlementResourceIds::Wood), 3);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FSettlementInventoryAtomicRecipeTest,
    "Settlement.Inventory.AtomicRecipeCost",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSettlementInventoryAtomicRecipeTest::RunTest(const FString& Parameters)
{
    UResourceInventoryComponent* Inventory = NewObject<UResourceInventoryComponent>();
    Inventory->AddResource(FSettlementResourceIds::Wood, 4);

    const TArray<FResourceAmount> TooExpensive = {
        FResourceAmount(FSettlementResourceIds::Wood, 2),
        FResourceAmount(FSettlementResourceIds::Stone, 1)
    };
    TestFalse(TEXT("Mixed cost is rejected when one resource is missing"), Inventory->RemoveResources(TooExpensive));
    TestEqual(TEXT("Failed atomic operation keeps wood"), Inventory->GetResourceAmount(FSettlementResourceIds::Wood), 4);

    const TArray<FResourceAmount> Affordable = {FResourceAmount(FSettlementResourceIds::Wood, 2)};
    TestTrue(TEXT("Affordable cost succeeds"), Inventory->RemoveResources(Affordable));
    TestEqual(TEXT("Successful operation removes exact amount"), Inventory->GetResourceAmount(FSettlementResourceIds::Wood), 2);
    return true;
}

#endif
