#include "HUD/GameplayWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UGameplayWidget::SetHealthProgress(const float& Percent) const
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void UGameplayWidget::SetStaminaProgress(const float& Percent) const
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void UGameplayWidget::SetGold(const int32& Value) const
{
	if (GoldText)
	{
		const FString String = FString::Printf(TEXT("%d"), Value);
		const FText Text = FText::FromString(String);
		GoldText->SetText(Text);
	}
}

void UGameplayWidget::SetSouls(const int32& Value) const
{
	if (SoulsText)
	{
		const FString String = FString::Printf(TEXT("%d"), Value);
		const FText Text = FText::FromString(String);
		SoulsText->SetText(Text);
	}
}