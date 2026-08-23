#include "Core/PrototypeGameMode.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavigationSystem.h"
#include "Player/SettlementPlayerCharacter.h"
#include "Resources/HarvestableResourceNode.h"
#include "Resources/ResourceTypes.h"
#include "UI/SettlementHUD.h"

APrototypeGameMode::APrototypeGameMode()
{
    DefaultPawnClass = ASettlementPlayerCharacter::StaticClass();
    HUDClass = ASettlementHUD::StaticClass();
}

void APrototypeGameMode::BeginPlay()
{
    Super::BeginPlay();
    if (bSpawnPrototypeSandbox)
    {
        SpawnSandbox();
    }
}

void APrototypeGameMode::SpawnSandbox()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    // Do not duplicate a designer-authored sandbox.
    for (TActorIterator<AHarvestableResourceNode> It(World); It; ++It)
    {
        return;
    }

    UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (Cube)
    {
        AStaticMeshActor* Ground = World->SpawnActor<AStaticMeshActor>(FVector(0.0f, 0.0f, -50.0f), FRotator::ZeroRotator);
        Ground->GetStaticMeshComponent()->SetStaticMesh(Cube);
        Ground->GetStaticMeshComponent()->SetWorldScale3D(FVector(30.0f, 30.0f, 1.0f));
        Ground->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
    }

    const TArray<FVector> TreeLocations = {
        FVector(700, -900, 0), FVector(900, -650, 0), FVector(1050, -300, 0), FVector(1100, 150, 0),
        FVector(950, 600, 0), FVector(650, 900, 0), FVector(-650, 950, 0), FVector(-1000, 650, 0),
        FVector(-1150, 250, 0), FVector(-1100, -250, 0), FVector(-900, -700, 0), FVector(-550, -1000, 0)
    };
    for (const FVector& Location : TreeLocations)
    {
        AHarvestableResourceNode* Node = World->SpawnActor<AHarvestableResourceNode>(Location, FRotator::ZeroRotator);
        if (Node)
        {
            Node->Configure(FSettlementResourceIds::Wood, 20);
        }
    }

    const TArray<FVector> StoneLocations = {
        FVector(1450, -850, 0), FVector(1550, -500, 0), FVector(1500, 450, 0), FVector(1300, 850, 0),
        FVector(-1400, 800, 0), FVector(-1550, 350, 0), FVector(-1500, -400, 0), FVector(-1250, -850, 0)
    };
    for (const FVector& Location : StoneLocations)
    {
        AHarvestableResourceNode* Node = World->SpawnActor<AHarvestableResourceNode>(Location, FRotator::ZeroRotator);
        if (Node)
        {
            Node->Configure(FSettlementResourceIds::Stone, 16);
        }
    }

    ANavMeshBoundsVolume* NavBounds = World->SpawnActor<ANavMeshBoundsVolume>(FVector(0.0f, 0.0f, 250.0f), FRotator::ZeroRotator);
    if (NavBounds)
    {
        NavBounds->SetActorScale3D(FVector(30.0f, 30.0f, 5.0f));
        if (UNavigationSystemV1* Navigation = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
        {
            Navigation->OnNavigationBoundsUpdated(NavBounds);
        }
    }

    ADirectionalLight* Sun = World->SpawnActor<ADirectionalLight>(FVector(0.0f, 0.0f, 1000.0f), FRotator(-45.0f, -35.0f, 0.0f));
    if (Sun)
    {
        Sun->GetLightComponent()->SetIntensity(5.0f);
    }
    ASkyLight* Sky = World->SpawnActor<ASkyLight>(FVector::ZeroVector, FRotator::ZeroRotator);
    if (Sky)
    {
        Sky->GetLightComponent()->SetIntensity(1.0f);
        Sky->GetLightComponent()->RecaptureSky();
    }

    if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0))
    {
        if (PlayerPawn->GetActorLocation().Z < 100.0f)
        {
            FVector SafeStart = PlayerPawn->GetActorLocation();
            SafeStart.Z = 150.0f;
            PlayerPawn->SetActorLocation(SafeStart, false, nullptr, ETeleportType::TeleportPhysics);
        }
    }
}
