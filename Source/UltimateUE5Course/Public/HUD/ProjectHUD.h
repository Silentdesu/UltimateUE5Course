// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ProjectHUD.generated.h"

class UGameplayWidget;
UCLASS()
class ULTIMATEUE5COURSE_API AProjectHUD : public AHUD
{
	GENERATED_BODY()

public:

	FORCEINLINE UGameplayWidget* GetGameplayWidget() const { return GameplayWidget; }

protected:

	virtual void BeginPlay() override;
	
private:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayWidget> GameplayWidgetSubclass;

	UPROPERTY()
	UGameplayWidget* GameplayWidget;
};
