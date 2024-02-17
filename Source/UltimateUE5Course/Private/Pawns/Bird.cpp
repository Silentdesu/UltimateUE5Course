#include "Pawns/Bird.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "UltimateUE5Course/Constants.h"

ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetCapsuleHalfHeight(20.0F);
	CapsuleComponent->SetCapsuleRadius(15.0F);
	SetRootComponent(CapsuleComponent);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(GetRootComponent());

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->TargetArmLength = 300.0F;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	bUseControllerRotationYaw = 1;
	bUseControllerRotationPitch = 1;
}

void ABird::BeginPlay()
{
	Super::BeginPlay();
}

void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(VERTICAL_AXIS, this, &ABird::VerticalInput);
	PlayerInputComponent->BindAxis(YAW_AXIS, this, &ABird::YawInput);
	PlayerInputComponent->BindAxis(PITCH_AXIS, this, &ABird::PitchInput);
}

void ABird::VerticalInput(const float Value)
{
	if (Controller && Value != 0.0F)
	{
		const FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, Value);
	}
}

void ABird::YawInput(const float Value)
{
	if (Controller && Value != 0.0F)
	{
		AddControllerYawInput(Value);
	}
}

void ABird::PitchInput(const float Value)
{
	if (Controller && Value != 0.0F)
	{
		AddControllerPitchInput(Value);
	}
}


