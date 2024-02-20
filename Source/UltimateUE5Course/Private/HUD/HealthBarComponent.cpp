#include "HUD/HealthBarComponent.h"
#include "Components/ProgressBar.h"
#include "HUD/HealthBar.h"

void UHealthBarComponent::BeginPlay()
{
	Super::BeginPlay();
	HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
}

void UHealthBarComponent::SetHealth(const float& Percent) const
{
	if (!(HealthBarWidget && HealthBarWidget->HealthBar)) return;
	HealthBarWidget->HealthBar->SetPercent(Percent);
}
