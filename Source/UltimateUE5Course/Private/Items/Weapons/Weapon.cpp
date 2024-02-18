#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "Interfaces/Hitable.h"
#include "Kismet/KismetSystemLibrary.h"

AWeapon::AWeapon()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(GetRootComponent());
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceStart"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceEnd"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());

	BoxHalfExtentSize = FVector(5.0F, 5.0F, 5.0F);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxBeginOverlap);
}

void AWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                   const FHitResult& SweepResult)
{
	Super::OnSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                const FHitResult& SweepResult)
{
	FVector Start = BoxTraceStart->GetComponentLocation();
	FVector End = BoxTraceEnd->GetComponentLocation();
	TArray<AActor*> ActorsToIgnore;
	FHitResult HitResult;

	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(Holder);

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.Add(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, BoxHalfExtentSize, BoxTraceStart->GetComponentRotation(),
	                                     TraceTypeQuery1, false, ActorsToIgnore,
	                                     EDrawDebugTrace::None, HitResult, true);

	if (HitResult.GetActor())
	{
		if (IHitable* Hitable = Cast<IHitable>(HitResult.GetActor()))
		{
			Hitable->Execute_GetHit(HitResult.GetActor(), HitResult.ImpactPoint);
		}
		IgnoreActors.AddUnique(HitResult.GetActor());
		CreateFields(HitResult.ImpactPoint);
	}
}
