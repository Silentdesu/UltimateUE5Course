#include "Characters/BaseCharacter.h"

#include "Components/AttributeComponent.h"
#include "Components/BoxComponent.h"
#include "Items/Weapons/Weapon.h"
#include "UltimateUE5Course/Constants.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = GetMesh()->GetAnimInstance();
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseCharacter::SetWeaponCollision(const ECollisionEnabled::Type Type)
{
	if (EquippedWeapon && EquippedWeapon->GetBoxComponent())
	{
		EquippedWeapon->GetBoxComponent()->SetCollisionEnabled(Type);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void ABaseCharacter::OnAttackEnd()
{
}

void ABaseCharacter::PerformAttack()
{
}

void ABaseCharacter::Die()
{
}

void ABaseCharacter::GetDirectionalHit(const FVector& ImpactPoint) const
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

	FName Section(HIT_REACT_FROM_BACK);

	if (Theta >= -45.0F && Theta < 45.0F)
	{
		Section = HIT_REACT_FROM_FRONT;
	}
	else if (Theta >= -135.0F && Theta < -45.0F)
	{
		Section = HIT_REACT_FROM_LEFT;
	}
	else if (Theta >= 45.0F && Theta < 135.0F)
	{
		Section = HIT_REACT_FROM_RIGHT;
	}

	PlayHitReactMontage(Section);
}

bool ABaseCharacter::CanAttack() const { return true; }

void ABaseCharacter::PlayAttackMontage() const
{
	if (AnimInstance && AttackMontage) AnimInstance->Montage_Play(AttackMontage);
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName) const
{
	if (HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}
