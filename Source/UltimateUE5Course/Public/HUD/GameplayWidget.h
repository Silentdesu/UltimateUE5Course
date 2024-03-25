#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class ULTIMATEUE5COURSE_API UGameplayWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetHealthProgress(const float& Percent) const;
	void SetStaminaProgress(const float& Percent) const;
	void SetGold(const int32& Value) const;
	void SetSouls(const int32& Value) const;
	
private:

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GoldText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SoulsText;
};
