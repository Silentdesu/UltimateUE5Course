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
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	FORCEINLINE
	void ApplyHealthChange(const float& Delta);

	FORCEINLINE
	float GetPercentage() const { return Health / MaxHealth; }

	FORCEINLINE
	bool IsAlive() const { return Health > 0.0F; }
	
protected:
	
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actor Properties", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actor Properties", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;
	
};
