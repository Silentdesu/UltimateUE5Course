#include "Characters/CourseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "UltimateUE5Course/Constants.h"

ACourseCharacter::ACourseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0F, 400.0F, 0.0F);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->TargetArmLength = 300.0F;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraBoom);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	bUseControllerRotationPitch = bUseControllerRotationRoll = bUseControllerRotationYaw = false;
}

void ACourseCharacter::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = GetMesh()->GetAnimInstance();
}

void ACourseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACourseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(VERTICAL_AXIS, this, &ACourseCharacter::PerformVerticalInput);
	PlayerInputComponent->BindAxis(HORIZONTAL_AXIS, this, &ACourseCharacter::PerformHorizontalInput);
	PlayerInputComponent->BindAxis(YAW_AXIS, this, &ACourseCharacter::PerformYawInput);
	PlayerInputComponent->BindAxis(PITCH_AXIS, this, &ACourseCharacter::PerformPitchInput);

	PlayerInputComponent->BindAction(JUMP_ACTION, IE_Pressed, this, &ACourseCharacter::PerformJump);
	PlayerInputComponent->BindAction(EQUIP_ACTION, IE_Pressed, this, &ACourseCharacter::PerformEquip);
	PlayerInputComponent->BindAction(ATTACK_ACTION, IE_Pressed, this, &ACourseCharacter::PerformAttack);
}

void ACourseCharacter::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), EquipmentSocketName);
	}
}

void ACourseCharacter::Disarm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), SPINE_SOCKET);
	}
}

void ACourseCharacter::FinishEquippingWeapon()
{
	ActionState = EActionState::EAC_Unoccuppied;
}

void ACourseCharacter::PerformVerticalInput(const float Value)
{
	if (!(Controller && Value != 0.0F && ActionState == EActionState::EAC_Unoccuppied)) return;

	const FVector Forward = GetDirection(EAxis::X);
	AddMovementInput(Forward, Value);
}

void ACourseCharacter::PerformHorizontalInput(const float Value)
{
	if (!(Controller && Value != 0.0F && ActionState == EActionState::EAC_Unoccuppied)) return;

	const FVector Right = GetDirection(EAxis::Y);
	AddMovementInput(Right, Value);
}

void ACourseCharacter::PerformYawInput(const float Value)
{
	if (!Controller) return;
	AddControllerYawInput(Value);
}

void ACourseCharacter::PerformPitchInput(const float Value)
{
	if (!Controller) return;
	AddControllerPitchInput(Value);
}

void ACourseCharacter::PerformJump()
{
	if (!Controller) return;
	Jump();
}

void ACourseCharacter::PerformEquip()
{
	if (AWeapon* Weapon = Cast<AWeapon>(OverlappingItem))
	{
		Weapon->Equip(GetMesh(), EquipmentSocketName);
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		OverlappingItem = nullptr;
		EquippedWeapon = Weapon;
	}
	else
	{
		const bool bCanDisarm = ActionState == EActionState::EAC_Unoccuppied &&
			CharacterState != ECharacterState::ECS_Unequipped;

		const bool bCanArm = ActionState == EActionState::EAC_Unoccuppied &&
			CharacterState == ECharacterState::ECS_Unequipped &&
			EquippedWeapon;
		
		if (bCanDisarm)
		{
			PlayEquipMontage(FName("Disarm"));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAC_EquippingWeapon;
		}
		else if (bCanArm)
		{
			PlayEquipMontage(FName("Arm"));
			CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			ActionState = EActionState::EAC_EquippingWeapon;
		}
	}
}

void ACourseCharacter::PerformAttack()
{
	const bool bCanAttack = ActionState == EActionState::EAC_Unoccuppied && CharacterState !=
		ECharacterState::ECS_Unequipped;
	if (!bCanAttack) return;

	PlayAttackMontage();
	ActionState = EActionState::EAC_Attack;
}

void ACourseCharacter::PlayAttackMontage() const
{
	if (AnimInstance && AttackMontage) AnimInstance->Montage_Play(AttackMontage);
}

void ACourseCharacter::PlayEquipMontage(const FName& SectionName) const
{
	if (EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ACourseCharacter::SetWeaponCollision(ECollisionEnabled::Type Type)
{
	if (EquippedWeapon && EquippedWeapon->GetBoxComponent())
	{
		EquippedWeapon->GetBoxComponent()->SetCollisionEnabled(Type);
	}
}