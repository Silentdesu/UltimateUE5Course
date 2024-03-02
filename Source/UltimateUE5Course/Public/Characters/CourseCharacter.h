#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "CourseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;
class AWeapon;
class UAnimMontage;

UCLASS()
class ULTIMATEUE5COURSE_API ACourseCharacter : public ABaseCharacter
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
	virtual bool CanAttack() const override;
	virtual void PerformAttack() override;
	virtual void OnAttackEnd() override;

	FORCEINLINE FVector GetDirection(EAxis::Type type) const
	{
		return FRotationMatrix(FRotator(0.0F, GetControlRotation().Yaw, 0.0F)).GetUnitAxis(type);
	}

	void PlayEquipMontage(const FName& SectionName) const;
	
private:
	
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

	EActionState ActionState = EActionState::EAC_Unoccuppied;
};
