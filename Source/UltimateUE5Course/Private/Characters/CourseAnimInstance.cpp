#include "Characters/CourseAnimInstance.h"
#include "Characters/CourseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UCourseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CourseCharacter = Cast<ACourseCharacter>(TryGetPawnOwner());

	if (CourseCharacter)
	{
		MovementComponent = CourseCharacter->GetCharacterMovement();
	}
}

void UCourseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (MovementComponent)
	{
		Velocity = UKismetMathLibrary::VSizeXY(MovementComponent->Velocity);
		bIsFalling = MovementComponent->IsFalling();
		CharacterState = CourseCharacter->GetCharacterState();
		ActionState = CourseCharacter->GetActionState();
		DeathPose = CourseCharacter->GetDeathPose();
	}
}

