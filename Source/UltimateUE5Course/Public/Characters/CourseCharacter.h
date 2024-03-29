#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "Interfaces/PickUp.h"
#include "CourseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;
class AWeapon;
class UAnimMontage;

UCLASS()
class ULTIMATEUE5COURSE_API ACourseCharacter : public ABaseCharacter, public IPickUp
{
	GENERATED_BODY()

public:
	ACourseCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void Jump() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Attacker) override;
	virtual void SetOverlappingItem(AItem* Item) override { OverlappingItem = Item; }
	virtual void AddSoul(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Treasure) override;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void Arm();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void Disarm();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetUnoccupiedState();

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }

protected:
	
	virtual void BeginPlay() override;
	virtual bool CanAttack() const override;
	virtual void PerformAttack() override;
	virtual void Die() override;
	
private:

	void PerformVerticalInput(const float Value);
	void PerformHorizontalInput(const float Value);
	void PerformYawInput(const float Value);
	void PerformPitchInput(const float Value);
	void PerformEquip();
	void PlayEquipMontage(const FName& SectionName) const;
	void EquipWeapon(AWeapon* Weapon);
	void InitializeHUD();

	FORCEINLINE FVector GetDirection(EAxis::Type type) const
	{
		return FRotationMatrix(FRotator(0.0F, GetControlRotation().Yaw, 0.0F)).GetUnitAxis(type);
	}

	FORCEINLINE void SetArmState(const FName& StateName, ECharacterState State)
	{
		PlayEquipMontage(StateName);
		CharacterState = State;
		ActionState = EActionState::EAC_EquippingWeapon;
	}
	
	FORCEINLINE bool IsUnoccupied() const { return ActionState == EActionState::EAC_Unoccuppied; }
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage;

	UPROPERTY(VisibleDefaultsOnly)
	FName EquipmentSocketName = "RightHandSocket";
	
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY()
	class UGameplayWidget* GameplayWidget;

	EActionState ActionState = EActionState::EAC_Unoccuppied;
};
