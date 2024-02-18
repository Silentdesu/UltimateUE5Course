#include "Breakables/BreakableProp.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

ABreakableProp::ABreakableProp()
{
	PrimaryActorTick.bCanEverTick = false;
	GeometryCollectionComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollectionComponent);
	GeometryCollectionComponent->SetGenerateOverlapEvents(true);
	GeometryCollectionComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void ABreakableProp::BeginPlay()
{
	Super::BeginPlay();
}

void ABreakableProp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABreakableProp::GetHit_Implementation(const FVector& ImpactPoint)
{
}

