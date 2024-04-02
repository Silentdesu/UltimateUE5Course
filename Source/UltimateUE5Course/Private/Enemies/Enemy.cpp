#include "Enemies/Enemy.h"
#include "AIController.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Items/Pickups/Soul.h"
#include "Items/Weapons/Weapon.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "UltimateUE5Course/Constants.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	InitialLifeSpan = 0.0F;

	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	WidgetComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("WidgetComp"));
	WidgetComponent->SetupAttachment(GetRootComponent());

	SphereComponent = CreateDefaultSubobject<USphereComponent>("AgroSphere");
	SphereComponent->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = bUseControllerRotationRoll = bUseControllerRotationYaw = false;

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("Pawn Sensing Component");
	PawnSensingComponent->SightRadius = 4000.0F;
	PawnSensingComponent->SetPeripheralVisionAngle(45.0F);
}


void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	WidgetComponent->SetHealth(1.0F);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnAgroSphereEndOverlap);
	SetHealthBarWidgetVisibility(false);

	AIController = Cast<AAIController>(GetController());
	MoveTo(PatrolTarget);

	PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemy::OnPawnSeen);

	UWorld* World = GetWorld();

	if (World && Weapon)
	{
		AWeapon* SpawnedWeapon = World->SpawnActor<AWeapon>(Weapon);
		SpawnedWeapon->Equip(GetMesh(), RIGHT_HAND_SOCKET, this, this);
		EquippedWeapon = SpawnedWeapon;
	}

	Tags.Add(ENEMY_TAG);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead()) return;

	CheckAction();
}

float AEnemy::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
                         AActor* DamageCauser)
{
	ApplyDamage(Damage);
	CombatTarget = EventInstigator->GetPawn();
	ClearTimer(AttackTimer);
	SetChasingState();

	return Damage;
}

void AEnemy::Destroyed()
{
	Super::Destroyed();

	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::OnPatrolState()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = GetPatrolTarget();
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::OnPatrolTimerFinished,
		                                FMath::RandRange(WaitPatrolMin, WaitPatrolMax));
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Attacker)
{
	Super::GetHit_Implementation(ImpactPoint, Attacker);

	if (!IsDead()) SetHealthBarWidgetVisibility(true);
	SetWeaponCollision(ECollisionEnabled::NoCollision);
	ClearTimer(PatrolTimer);
	ClearTimer(AttackTimer);
	StopMontage(AttackMontage);
}

void AEnemy::Die()
{
	if (ActionState == EEnemyState::EES_Dead) return;

	Super::Die();
	ActionState = EEnemyState::EES_Dead;
	SetLifeSpan(DeathLifeSpan);
	SetHealthBarWidgetVisibility(false);
	ClearTimer(AttackTimer);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetCapsuleCollision(ECollisionEnabled::NoCollision);
	SetWeaponCollision(ECollisionEnabled::NoCollision);
	SpawnSoul();
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
	AIController->MoveTo(MoveRequest);
}

void AEnemy::OnPatrolTimerFinished() const
{
	MoveTo(PatrolTarget);
}

void AEnemy::OnPawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		!IsDead() && !IsChasing() && ActionState < EEnemyState::EES_Attacking && SeenPawn->ActorHasTag(PLAYER_TAG);

	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearTimer(PatrolTimer);
		ClearTimer(AttackTimer);
		if (!IsEngaged()) SetChasingState();
	}
}

void AEnemy::OnAgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsDead()) return;

	if (OtherActor == CombatTarget)
	{
		ClearTimer(AttackTimer);
		LoseInterest();
		if (!IsEngaged()) SetPatrollingState();
	}
}

void AEnemy::SetMaxWalkSpeed(const float& NewSpeed) const
{
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

void AEnemy::SpawnSoul() const
{
	UWorld* World = GetWorld();
	if (World && SoulClass)
	{
		if (ASoul* Soul = World->SpawnActor<ASoul>(SoulClass, GetActorLocation(), GetActorRotation()))
		{
			Soul->SetSouls(AttributeComponent->GetSouls());
		}
	}
}

void AEnemy::PerformAttack()
{
	Super::PerformAttack();

	if (CombatTarget == nullptr) return;
	ActionState = EEnemyState::EES_Engaged;
}

void AEnemy::ApplyDamage(const float& Damage)
{
	Super::ApplyDamage(Damage);
	WidgetComponent->SetHealth(AttributeComponent->GetHealthPercentage());
}

void AEnemy::OnAttackEnd()
{
	ActionState = EEnemyState::EES_None;
	if (CombatTarget == nullptr)
	{
		SetPatrollingState();
		return;
	}

	CheckAction();
}