#include "Enemies/Enemy.h"
#include "AIController.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"
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
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActionState == EEnemyState::EES_Patrolling)
	{
		OnPatrolState();
	}

	if (InTargetRange(CombatTarget, AttackRadius) && ActionState != EEnemyState::EES_Attacking)
	{
		ActionState = EEnemyState::EES_Attacking;
	}
}

float AEnemy::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
                         AActor* DamageCauser)
{
	AttributeComponent->ApplyHealthChange(Damage);
	WidgetComponent->SetHealth(AttributeComponent->GetPercentage());
	CombatTarget = EventInstigator->GetPawn();
	SetMaxWalkSpeed(AgroMaxSpeed);
	ActionState = EEnemyState::EES_Chasing;
	MoveTo(CombatTarget);
	
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
		GetWorldTimerManager().SetTimer(PatrolTimerHandle, this, &AEnemy::OnPatrolTimerFinished,
		                                FMath::RandRange(WaitPatrolMin, WaitPatrolMax));
	}
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
	MoveTo(PatrolTarget);
}

void AEnemy::OnPawnSeen(APawn* SeenPawn)
{
	if (!(ActionState != EEnemyState::EES_Chasing && SeenPawn->ActorHasTag(PLAYER_TAG))) return;

	GetWorldTimerManager().ClearTimer(PatrolTimerHandle);
	SetMaxWalkSpeed(AgroMaxSpeed);
	CombatTarget = SeenPawn;

	if (ActionState != EEnemyState::EES_Attacking)
	{
		ActionState = EEnemyState::EES_Chasing;
		MoveTo(SeenPawn);
	}
}

void AEnemy::OnAgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == CombatTarget)
	{
		CombatTarget = nullptr;
		WidgetComponent->SetVisibility(false);
		ActionState = EEnemyState::EES_Patrolling;
		SetMaxWalkSpeed(DefaultMaxSpeed);
		MoveTo(PatrolTarget);
	}
}

void AEnemy::SetMaxWalkSpeed(const float& NewSpeed) const
{
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}