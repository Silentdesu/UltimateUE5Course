#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "HUD/HealthBarComponent.h"
#include "Enemy.generated.h"

class UPawnSensingComponent;
class AAIController;
class USphereComponent;
class UHealthBarComponent;
class AWeapon;

UCLASS()
class ULTIMATEUE5COURSE_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Attacker) override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	void CheckAction()
	{
		if (IsPatrolling())
		{
			OnPatrolState();
		}
		else if (CanAttack())
		{
			if (!IsEngaged()) SetAttackState();
		}
	}

	virtual void Die() override;
	virtual void PerformAttack() override;
	virtual void ApplyDamage(const float& Damage) override;
	virtual void OnAttackEnd() override;

	void OnPatrolState();
	AActor* GetPatrolTarget();
	bool InTargetRange(const AActor* Target, const float& AcceptanceRadius) const;
	void MoveTo(const AActor* Target, const float& AcceptanceRadius = 15.0F) const;
	void OnPatrolTimerFinished() const;
	void SetMaxWalkSpeed(const float& NewSpeed) const;

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	UFUNCTION()
	void OnAgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FORCEINLINE
	void SetHealthBarWidgetVisibility(const bool& IsVisible) const { WidgetComponent->SetVisibility(IsVisible); }

private:
	void SetPatrollingState()
	{
		ActionState = EEnemyState::EES_Patrolling;
		SetMaxWalkSpeed(DefaultMaxSpeed);
		MoveTo(PatrolTarget);
	}

	void SetChasingState()
	{
		ActionState = EEnemyState::EES_Chasing;
		SetMaxWalkSpeed(AgroMaxSpeed);
		MoveTo(CombatTarget, CombatTargetRadius);
	}

	void SetAttackState()
	{
		ActionState = EEnemyState::EES_Attacking;
		const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::PerformAttack, AttackTime);
	}

	void LoseInterest()
	{
		CombatTarget = nullptr;
		SetHealthBarWidgetVisibility(false);
	}

	bool IsPatrolling() const { return ActionState == EEnemyState::EES_Patrolling; }
	bool IsChasing() const { return ActionState == EEnemyState::EES_Chasing; }
	bool IsAttacking() const { return ActionState == EEnemyState::EES_Attacking; }
	bool IsEngaged() const { return ActionState == EEnemyState::EES_Engaged; }

	virtual bool CanAttack() const override
	{
		return IsInsideAttackRadius() && !IsAttacking() && !IsEngaged() && !IsDead();
	}

	bool IsInsideAttackRadius() const { return InTargetRange(CombatTarget, AttackRadius); }
	bool IsOutsideAttackRadius() const { return !IsInsideAttackRadius(); }
	bool IsDead() const { return ActionState == EEnemyState::EES_Dead; }

	FORCEINLINE void ClearTimer(FTimerHandle& TimerHandle) const { GetWorldTimerManager().ClearTimer(TimerHandle); }

private:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* WidgetComponent;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> Weapon;

	UPROPERTY()
	AAIController* AIController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float CombatTargetRadius = 60.0F;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolRadius = 200.0F;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float AttackRadius = 150.0F;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	int32 WaitPatrolMin = 5;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	int32 WaitPatrolMax = 10;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float AgroMaxSpeed = 300.0F;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float DefaultMaxSpeed = 150.0F;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = 0.5F;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.0F;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EEnemyState ActionState = EEnemyState::EES_Patrolling;

	FTimerHandle PatrolTimer;
	FTimerHandle AttackTimer;
};
