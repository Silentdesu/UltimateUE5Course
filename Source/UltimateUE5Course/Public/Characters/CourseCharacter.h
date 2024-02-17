#pragma once

#include "CoreMinimal.h"
#include "CharacterTypes.h"
#include "GameFramework/Character.h"
#include "CourseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;
class AWeapon;
class UAnimMontage;

UCLASS()
class ULTIMATEUE5COURSE_API ACourseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACourseCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void Arm();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void Disarm();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void FinishEquippingWeapon();

protected:
	virtual void BeginPlay() override;

	void PerformVerticalInput(const float Value);
	void PerformHorizontalInput(const float Value);
	void PerformYawInput(const float Value);
	void PerformPitchInput(const float Value);
	void PerformJump();
	void PerformEquip();
	void PerformAttack();
	void PlayAttackMontage() const;

	FORCEINLINE FVector GetDirection(EAxis::Type type) const
	{
		return FRotationMatrix(FRotator(0.0F, GetControlRotation().Yaw, 0.0F)).GetUnitAxis(type);
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void OnAttackEnd() { ActionState = EActionState::EAC_Unoccuppied; }

	void PlayEquipMontage(const FName& SectionName) const;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(const ECollisionEnabled::Type Type);
	
private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	UPROPERTY()
	UAnimInstance* AnimInstance;
	
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage;

	UPROPERTY(VisibleDefaultsOnly)
	FName EquipmentSocketName = "RightHandSocket";
	
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(VisibleAnywhere, Category = "Weapons")
	AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	EActionState ActionState = EActionState::EAC_Unoccuppied;
};
