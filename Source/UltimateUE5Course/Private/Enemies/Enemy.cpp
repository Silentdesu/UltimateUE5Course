#include "Enemies/Enemy.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Kismet/GameplayStatics.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));

	WidgetComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("WidgetComp"));
	WidgetComponent->SetupAttachment(GetRootComponent());

	SphereComponent = CreateDefaultSubobject<USphereComponent>("AgroSphere");
	SphereComponent->SetupAttachment(GetRootComponent());
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = GetMesh()->GetAnimInstance();
	WidgetComponent->SetHealth(1.0F);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnAgroSphereEndOverlap);
	SetHealthBarWidgetVisibility(false);
}

void AEnemy::PlayHitReactMontage(const FName& SectionName) const
{
	if (HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	SetHealthBarWidgetVisibility(true);

	if (AttributeComponent->IsAlive())
	{
		GetDirectionalHit(ImpactPoint);
	}
	else
	{
		Die();
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}

	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, ImpactPoint);
	}
}


void AEnemy::GetDirectionalHit(const FVector& ImpactPoint) const
{
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();
	double Theta = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Forward, ToHit)));
	const FVector Cross = FVector::CrossProduct(Forward, ToHit);

	if (Cross.Z < 0.0F)
	{
		Theta *= -1.0F;
	}

	FName Section("FromBack");

	if (Theta >= -45.0F && Theta < 45.0F)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.0F && Theta < -45.0F)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.0F && Theta < 135.0F)
	{
		Section = FName("FromRight");
	}

	PlayHitReactMontage(Section);
}

void AEnemy::Die()
{
	if (DeathMontage && DeathAnimations.Num() > 0 && DeathPosses.Num() > 0)
	{
		const int32 Section = FMath::RandRange(0, DeathAnimations.Num() - 1);
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(DeathAnimations[Section], DeathMontage);
		DeathPose = DeathPosses[Section];
		WidgetComponent->SetVisibility(false);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetLifeSpan(LifeSpan);
	}
}

float AEnemy::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
                         AActor* DamageCauser)
{
	AttributeComponent->ApplyHealthChange(Damage);
	WidgetComponent->SetHealth(AttributeComponent->GetPercentage());
	CombatTarget = EventInstigator->GetPawn();
	return Damage;
}

void AEnemy::OnAgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == CombatTarget)
	{
		CombatTarget = nullptr;
		WidgetComponent->SetVisibility(false);
	}
}
