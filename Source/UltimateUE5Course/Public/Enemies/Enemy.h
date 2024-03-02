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
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class ULTIMATEUE5COURSE_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;
	void OnPatrolState();
	virtual void Die() override;
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

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensingComponent;


	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* WidgetComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	TMap<EDeathPose, FName> DeathAnimationsMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EDeathPose DeathPose;

	UPROPERTY(EditAnywhere, Category = "Death")
	float LifeSpan = 3.0F;

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY()
	AAIController* AIController;
	
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

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

	EEnemyState ActionState = EEnemyState::EES_Patrolling;
	FTimerHandle PatrolTimerHandle;
};
