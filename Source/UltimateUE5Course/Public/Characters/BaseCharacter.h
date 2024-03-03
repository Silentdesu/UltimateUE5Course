#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/Hitable.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class ULTIMATEUE5COURSE_API ABaseCharacter : public ACharacter, public IHitable
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;
	virtual bool CanAttack() const;
	virtual void PerformAttack(); 
	virtual void Die();
	void GetDirectionalHit(const FVector& ImpactPoint) const;

	/*
	 * Montages
	 */
	
	virtual void PlayAttackMontage() const;
	void PlayHitReactMontage(const FName& SectionName) const;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(const ECollisionEnabled::Type Type);

	UFUNCTION(BlueprintCallable)
	virtual void OnAttackEnd();

protected:

	/*
	 * Components
	 */
	
	UPROPERTY(VisibleAnywhere, Category = "Weapons")
	AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* AttributeComponent;

	UPROPERTY()
	UAnimInstance* AnimInstance;
	
	/*
	 * Montages
	 */
	
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TArray<FName> Attacks;
	
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	/*
	 * SFX & VFX
	 */
	
	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "VFX")
	UParticleSystem* HitParticle;
};
