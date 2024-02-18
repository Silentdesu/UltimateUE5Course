// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/Hitable.h"
#include "Enemy.generated.h"

class UAnimMontage;

UCLASS()
class ULTIMATEUE5COURSE_API AEnemy : public ACharacter, public IHitable
{
	GENERATED_BODY()

public:

	AEnemy();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit(const FVector& ImpactPoint) override;
	
protected:

	virtual void BeginPlay() override;
	void PlayHitReactMontage(const FName& SectionName) const;
	void GetDirectionalHit(const FVector& ImpactPoint) const;

private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(VisibleAnywhere)
	UAnimInstance* AnimInstance;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "VFX")
	UParticleSystem* HitParticle;
};
