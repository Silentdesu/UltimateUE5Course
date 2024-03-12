#include "Characters/BaseCharacter.h"
#include "Components/AttributeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
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

void ABaseCharacter::ApplyDamage(const float& Damage)
{
	AttributeComponent->ApplyHealthChange(Damage);
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (AttributeComponent->IsAlive())GetDirectionalHit(ImpactPoint);
	else Die();
	PlayHitSound(ImpactPoint);
	PlayHitParticles(ImpactPoint);
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

void ABaseCharacter::SetCapsuleCollision(ECollisionEnabled::Type Type) const
{
	GetCapsuleComponent()->SetCollisionEnabled(Type);
}

int32 ABaseCharacter::PlayRandomMontageBySections(UAnimMontage* Montage, const TArray<FName>& Sections) const
{
	if (Sections.Num() <= 0) return -1;

	const int32 Idx = FMath::RandRange(0, Sections.Num() - 1);
	PlayMontageSection(Montage, Sections[Idx]);
	return Idx;
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName Section) const
{
	if (!Montage) return;
	
	AnimInstance->Montage_Play(Montage);
	AnimInstance->Montage_JumpToSection(Section);
}

bool ABaseCharacter::CanAttack() const { return true; }

int32 ABaseCharacter::PlayAttackMontage() const
{
	return PlayRandomMontageBySections(AttackMontage, AttackMontageSections);
}

int32 ABaseCharacter::PlayDeathMontage()
{
	return PlayRandomMontageBySections(DeathMontage, DeathMontageSections);
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName) const
{
	PlayMontageSection(HitReactMontage, SectionName);
}
