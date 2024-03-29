#include "Items/Weapons/Weapon.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Interfaces/Hitable.h"
#include "Kismet/GameplayStatics.h"
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

void AWeapon::Equip(USceneComponent* Parent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(Parent, InSocketName);
	Holder = Parent->GetAttachParentActor();
	State = EItemState::EIS_Equipped;

	SpawnEquipSFX();

	if (SphereComponent)
	{
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (ItemEffect)
	{
		ItemEffect->Deactivate();
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxBeginOverlap);
}


void AWeapon::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                const FHitResult& SweepResult)
{
	if (IsAlly(OtherActor)) return;

	FHitResult HitResult;
	BoxTrace(HitResult);

	if (HitResult.GetActor())
	{
		if (IsAlly(HitResult.GetActor())) return;
		
		UGameplayStatics::ApplyDamage(HitResult.GetActor(), Damage, GetInstigator()->GetController(),
		                              this, UDamageType::StaticClass());

		ExecuteGetHit(HitResult);
		CreateFields(HitResult.ImpactPoint);
	}
}

void AWeapon::BoxTrace(FHitResult& HitResult)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();
	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(Holder);

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.Add(Actor);
	}

	const EDrawDebugTrace::Type Trace = bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, BoxHalfExtentSize, BoxTraceStart->GetComponentRotation(),
	                                     TraceTypeQuery1, false, ActorsToIgnore,
	                                     Trace, HitResult, true);

	IgnoreActors.AddUnique(HitResult.GetActor());
}

void AWeapon::ExecuteGetHit(const FHitResult& HitResult) const
{
	if (IHitable* Hitable = Cast<IHitable>(HitResult.GetActor()))
	{
		Hitable->Execute_GetHit(HitResult.GetActor(), HitResult.ImpactPoint, GetOwner());
	}
}
