#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterTypes.h"
#include "GameFramework/Character.h"
#include "HUD/HealthBarComponent.h"
#include "Interfaces/Hitable.h"
#include "Enemy.generated.h"

class UPawnSensingComponent;
class AAIController;
class USphereComponent;
class UHealthBarComponent;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class ULTIMATEUE5COURSE_API AEnemy : public ACharacter, public IHitable
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;
	void PlayHitReactMontage(const FName& SectionName) const;
	void OnPatrolState();
	void GetDirectionalHit(const FVector& ImpactPoint) const;
	void Die();
	AActor* GetPatrolTarget();
	bool InTargetRange(const AActor* Target, const float& AcceptanceRadius) const;
	void MoveTo(const AActor* Target, const float& AcceptanceRadius = 15.0F) const;
	void OnPatrolTimerFinished() const;

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	UFUNCTION()
	void OnAgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FORCEINLINE
	void SetHealthBarWidgetVisibility(const bool& IsVisible) const { WidgetComponent->SetVisibility(IsVisible); }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(VisibleAnywhere)
	UAnimInstance* AnimInstance;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "VFX")
	UParticleSystem* HitParticle;

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* AttributeComponent;

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
