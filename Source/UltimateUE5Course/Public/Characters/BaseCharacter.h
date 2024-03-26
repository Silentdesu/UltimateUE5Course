#pragma once

#include "CoreMinimal.h"
#include "CharacterTypes.h"
#include "GameFramework/Character.h"
#include "Interfaces/Hitable.h"
#include "Kismet/GameplayStatics.h"
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

	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }

protected:

	virtual void BeginPlay() override;
	virtual bool CanAttack() const;
	virtual void PerformAttack(); 
	virtual void Die();
	virtual void ApplyDamage(const float& Damage);
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Attacker) override;
	void GetDirectionalHit(const FVector& ImpactPoint) const;
	void SetCapsuleCollision(ECollisionEnabled::Type Type) const;
	void DisableMeshCollision() const;

	/*
	 * Montages
	 */

	void PlayMontageSection(UAnimMontage* Montage, const FName Section) const;
	int32 PlayRandomMontageBySections(UAnimMontage* Montage, const TArray<FName>& Sections) const;
	virtual int32 PlayAttackMontage() const;
	virtual int32 PlayDeathMontage();
	void PlayHitReactMontage(const FName& SectionName) const;
	void StopMontage(const UAnimMontage* Montage, const float& BlendOut = 0.0F) const;

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	/*
	 * BlueprintCallables
	 */
	
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(const ECollisionEnabled::Type Type);

	UFUNCTION(BlueprintCallable)
	virtual void OnAttackEnd();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void PlayHitSound(const FVector& ImpactPoint)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void PlayHitParticles(const FVector& ImpactPoint)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, ImpactPoint);
	}

protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Combat")
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double WarpTargetDistance = 75.0F;
	
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
	TArray<FName> AttackMontageSections;
	
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TArray<FName> DeathMontageSections;

	UPROPERTY(EditAnywhere, Category = "Montages")
	float StopMontageBlendOut = 0.25F;

	/*
	 * SFX & VFX
	 */
	
	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "VFX")
	UParticleSystem* HitParticle;

	UPROPERTY(EditAnywhere, Category = "Death")
	float DeathLifeSpan = 10.0F;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EDeathPose> DeathPose;
};
