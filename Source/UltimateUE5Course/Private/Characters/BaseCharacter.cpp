#include "Characters/BaseCharacter.h"

#include "Characters/CharacterTypes.h"
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
	if (CombatTarget != nullptr && CombatTarget->ActorHasTag(DEAD_TAG))
	{
		CombatTarget = nullptr;
		return;
	}
	
	PlayAttackMontage();
}

void ABaseCharacter::Die()
{
	Tags.Add(DEAD_TAG);
	PlayDeathMontage();
}

void ABaseCharacter::ApplyDamage(const float& Damage)
{
	AttributeComponent->ApplyHealthChange(Damage);
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Attacker)
{
	if (AttributeComponent->IsAlive() && Attacker)GetDirectionalHit(Attacker->GetActorLocation());
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

void ABaseCharacter::DisableMeshCollision() const
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	const int32 Idx = PlayRandomMontageBySections(DeathMontage, DeathMontageSections);
	const TEnumAsByte<EDeathPose> Pose(Idx);
	if (Pose < EDP_MAX)
	{
		DeathPose = Pose;
	}

	return Idx;
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName) const
{
	PlayMontageSection(HitReactMontage, SectionName);
}

void ABaseCharacter::StopMontage(const UAnimMontage* Montage, const float& BlendOut) const
{
	const float InBlendOut = BlendOut == 0.25F ? BlendOut : StopMontageBlendOut;
	AnimInstance->Montage_Stop(InBlendOut, Montage);
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr) return FVector();

	const FVector Target = CombatTarget->GetActorLocation();
	const FVector Attacker = GetActorLocation();

	const FVector Diff = (Attacker - Target).GetSafeNormal() * WarpTargetDistance;
	return Target + Diff;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	return CombatTarget == nullptr ? FVector() : CombatTarget->GetActorLocation();
}
