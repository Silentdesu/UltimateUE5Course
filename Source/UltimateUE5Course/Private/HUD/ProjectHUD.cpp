#include "HUD/ProjectHUD.h"
#include "Blueprint/UserWidget.h"
#include "HUD/GameplayWidget.h"

void AProjectHUD::BeginPlay()
{
	Super::BeginPlay();

	if (const UWorld* World = GetWorld())
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && GameplayWidgetSubclass)
		{
			GameplayWidget = CreateWidget<UGameplayWidget>(Controller, GameplayWidgetSubclass);
			GameplayWidget->AddToViewport();
		}
	}
}
