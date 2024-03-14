#include "Breakables/BreakableProp.h"

#include "Components/CapsuleComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Pickups/Treasure.h"

ABreakableProp::ABreakableProp()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollectionComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollectionComponent);
	GeometryCollectionComponent->SetGenerateOverlapEvents(true);
	GeometryCollectionComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GeometryCollectionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetupAttachment(GetRootComponent());
	CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

void ABreakableProp::BeginPlay()
{
	Super::BeginPlay();
}

void ABreakableProp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABreakableProp::GetHit_Implementation(const FVector& ImpactPoint, AActor* Attacker)
{
	if (bHasBroken) return;
	
	bHasBroken = true;
	UWorld* World = GetWorld();
	if (World && Treasures.Num() > 0)
	{
		FVector Location = GetActorLocation();
		Location.Z += 75.0F;

		const int32 Idx = FMath::RandRange(0, Treasures.Num() - 1);
		World->SpawnActor<ATreasure>(Treasures[Idx], Location, GetActorRotation());
	}
}
