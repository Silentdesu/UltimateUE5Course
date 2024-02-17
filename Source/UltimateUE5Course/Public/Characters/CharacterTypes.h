#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAC_Unoccuppied UMETA(DisplayName = "Unoccupied"),
	EAC_Attack UMETA(DisplayName = "Attack"),
	EAC_EquippingWeapon UMETA(DisplayName = "Equipping Weapon")
};