#include "Enemies/Enemy.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = GetMesh()->GetAnimInstance();
}

void AEnemy::PlayHitReactMontage(const FName& SectionName) const
{
	if (HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::GetHit(const FVector& ImpactPoint)
{
	GetDirectionalHit(ImpactPoint);
}

void AEnemy::GetDirectionalHit(const FVector& ImpactPoint) const
{
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();
	double Theta = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Forward, ToHit)));
	const FVector Cross = FVector::CrossProduct(Forward, ToHit);

	if (Cross.Z < 0.0F)
	{
		Theta *= -1.0F;
	}

	FName Section("FromBack");

	if (Theta >= -45.0F && Theta < 45.0F)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.0F && Theta < -45.0F)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.0F && Theta < 135.0F)
	{
		Section = FName("FromRight");
	}

	PlayHitReactMontage(Section);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.0F, FColor::Green,
		                                 FString::Printf(TEXT("Theta: %f"), Theta));
	}

	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(),
	                                     GetActorLocation() + Forward * 100.0F, 5.0F, FLinearColor::Red, 5.0F);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(),
	                                     GetActorLocation() + ToHit * 100.0F, 5.0F, FLinearColor::Green, 5.0F);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(),
	                                     GetActorLocation() + Cross * 100.0F, 5.0F, FLinearColor::Blue, 5.0F);
}
