#include "Characters/CourseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/AttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HUD/GameplayWidget.h"
#include "HUD/ProjectHUD.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "UltimateUE5Course/Constants.h"

ACourseCharacter::ACourseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0F, 400.0F, 0.0F);

	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	GetMesh()->SetGenerateOverlapEvents(true);

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

	Tags.Add(PLAYER_TAG);
	InitializeHUD();
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

	PlayerInputComponent->BindAction(JUMP_ACTION, IE_Pressed, this, &ACourseCharacter::Jump);
	PlayerInputComponent->BindAction(EQUIP_ACTION, IE_Pressed, this, &ACourseCharacter::PerformEquip);
	PlayerInputComponent->BindAction(ATTACK_ACTION, IE_Pressed, this, &ACourseCharacter::PerformAttack);
}

float ACourseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                   AActor* DamageCauser)
{
	ApplyDamage(DamageAmount);
	GameplayWidget->SetHealthProgress(AttributeComponent->GetHealthPercentage());
	return DamageAmount;
}

void ACourseCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}
}

void ACourseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Attacker)
{
	Super::GetHit_Implementation(ImpactPoint, Attacker);

	if (AttributeComponent->IsAlive()) ActionState = EActionState::EAC_HitReact;
	SetWeaponCollision(ECollisionEnabled::NoCollision);
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

void ACourseCharacter::SetUnoccupiedState()
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

void ACourseCharacter::PerformEquip()
{
	if (AWeapon* Weapon = Cast<AWeapon>(OverlappingItem))
	{
		EquipWeapon(Weapon);
	}
	else
	{
		const bool bCanArm = ActionState == EActionState::EAC_Unoccuppied &&
			CharacterState == ECharacterState::ECS_Unequipped &&
			EquippedWeapon;

		if (bCanArm)
		{
			SetArmState(ARM_STATE, ECharacterState::ECS_EquippedOneHandedWeapon);
		}
		else
		{
			SetArmState(DISARM_STATE, ECharacterState::ECS_Unequipped);
		}
	}
}

bool ACourseCharacter::CanAttack() const
{
	return ActionState == EActionState::EAC_Unoccuppied && CharacterState !=
		ECharacterState::ECS_Unequipped;
}

void ACourseCharacter::PerformAttack()
{
	if (!CanAttack()) return;

	ActionState = EActionState::EAC_Attack;
	Super::PerformAttack();
}

void ACourseCharacter::Die()
{
	Super::Die();

	ActionState = EActionState::EAC_Dead;
	DisableMeshCollision();
}

void ACourseCharacter::PlayEquipMontage(const FName& SectionName) const
{
	if (EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ACourseCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), EquipmentSocketName, this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}

void ACourseCharacter::InitializeHUD()
{
	if (const APlayerController* Player = Cast<APlayerController>(GetController()))
	{
		if (const AProjectHUD* ProjectHUD = Cast<AProjectHUD>(Player->GetHUD()))
		{
			GameplayWidget = ProjectHUD->GetGameplayWidget();

			if (GameplayWidget && AttributeComponent)
			{
				GameplayWidget->SetHealthProgress(AttributeComponent->GetHealthPercentage());
				GameplayWidget->SetStaminaProgress(1.0F);
				GameplayWidget->SetGold(0);
				GameplayWidget->SetSouls(0);
			}
		}
	}
}
