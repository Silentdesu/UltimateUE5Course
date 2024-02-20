#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

class UHealthBar;

UCLASS()
class ULTIMATEUE5COURSE_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	void SetHealth(const float& Percent) const;

private:

	UPROPERTY(VisibleAnywhere)
	UHealthBar* HealthBarWidget;
};
