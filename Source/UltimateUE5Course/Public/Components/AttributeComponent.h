#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ULTIMATEUE5COURSE_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UAttributeComponent();

public:

	FORCEINLINE
	void ApplyHealthChange(const float& Delta) { Health = FMath::Clamp(Health - Delta, 0.0f, MaxHealth); }

	FORCEINLINE
	void UseStamina(const float& Delta) { Stamina = FMath::Clamp(Stamina - Delta, 0.0f, MaxStamina); }

	FORCEINLINE
	void RegenStamina(const float& DeltaTime) { Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0, MaxStamina); }

	FORCEINLINE
	float GetHealthPercentage() const { return Health / MaxHealth; }

	FORCEINLINE
	float GetStaminaPercentage() const { return Stamina / MaxStamina; }

	FORCEINLINE
	bool IsAlive() const { return Health > 0.0F; }

	FORCEINLINE
	void SetGold(const int32& Value) { Gold += Value; }

	FORCEINLINE
	int32 GetGold() const { return Gold; }

	FORCEINLINE
	void SetSouls(const int32& Value) { Souls += Value; }
	
	FORCEINLINE
	int32 GetSouls() const { return Souls; }

	FORCEINLINE
	float GetStamina() const { return Stamina; }

	FORCEINLINE
	float GetDodgeCost() const { return DodgeCost; }
	
protected:
	
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actor Properties", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actor Properties", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actor Properties", meta = (AllowPrivateAccess = "true"))
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actor Properties", meta = (AllowPrivateAccess = "true"))
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actor Properties", meta = (AllowPrivateAccess = "true"))
	float StaminaRegenRate = 2.0F;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actor Properties", meta = (AllowPrivateAccess = "true"))
	float DodgeCost = 14.0F;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actor Properties", meta = (AllowPrivateAccess = "true"))
	int32 Gold;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actor Properties", meta = (AllowPrivateAccess = "true"))
	int32 Souls;
};
