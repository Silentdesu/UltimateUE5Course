#include "Enemies/Enemy.h"

#include "AIController.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "UltimateUE5Course/Constants.h"

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

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = bUseControllerRotationRoll = bUseControllerRotationYaw = false;
}


void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = GetMesh()->GetAnimInstance();
	WidgetComponent->SetHealth(1.0F);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnAgroSphereEndOverlap);
	SetHealthBarWidgetVisibility(false);

	AIController = Cast<AAIController>(GetController());
	MoveTo(PatrolTarget, 15.0F);
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

	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = GetPatrolTarget();
		GetWorldTimerManager().SetTimer(PatrolTimerHandle, this, &AEnemy::OnPatrolTimerFinished,
		                                FMath::RandRange(WaitPatrolMin, WaitPatrolMax));
	}
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

	FName Section(ENEMY_HIT_REACT_FROM_BACK);

	if (Theta >= -45.0F && Theta < 45.0F)
	{
		Section = ENEMY_HIT_REACT_FROM_FRONT;
	}
	else if (Theta >= -135.0F && Theta < -45.0F)
	{
		Section = ENEMY_HIT_REACT_FROM_LEFT;
	}
	else if (Theta >= 45.0F && Theta < 135.0F)
	{
		Section = ENEMY_HIT_REACT_FROM_RIGHT;
	}

	PlayHitReactMontage(Section);
}

void AEnemy::Die()
{
	if (DeathMontage && DeathAnimationsMap.Num() > 0)
	{
		const uint8 Section = FMath::RandRange(static_cast<uint8>(EDeathPose::EDP_Death1),
		                                       static_cast<uint8>(EDeathPose::EDP_Death5));
		const EDeathPose DeathType = static_cast<EDeathPose>(Section);
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(DeathAnimationsMap[DeathType], DeathMontage);
		DeathPose = DeathType;
		WidgetComponent->SetVisibility(false);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetLifeSpan(LifeSpan);
	}
}

AActor* AEnemy::GetPatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target == PatrolTarget) continue;
		ValidTargets.AddUnique(Target);
	}

	const int32 TargetNumber = ValidTargets.Num();

	if (TargetNumber > 0)
	{
		return PatrolTargets[FMath::RandRange(0, TargetNumber - 1)];
	}

	return nullptr;
}

bool AEnemy::InTargetRange(const AActor* Target, const float& AcceptanceRadius) const
{
	if (Target == nullptr) return false;
	return (Target->GetActorLocation() - GetActorLocation()).Size() <= AcceptanceRadius;
}

void AEnemy::MoveTo(const AActor* NewTarget, const float& AcceptanceRadius) const
{
	if (!(AIController && NewTarget)) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(NewTarget);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	FNavPathSharedPtr OutPath;
	AIController->MoveTo(MoveRequest, &OutPath);
}

void AEnemy::OnPatrolTimerFinished() const
{
	MoveTo(PatrolTarget, 15.0F);
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
