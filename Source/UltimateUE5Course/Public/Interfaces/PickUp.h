#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickUp.generated.h"

UINTERFACE(MinimalAPI)
class UPickUp : public UInterface
{
	GENERATED_BODY()
};

class ULTIMATEUE5COURSE_API IPickUp
{
	GENERATED_BODY()

public:

	virtual void SetOverlappingItem(class AItem* Item);
	virtual void AddSoul(class ASoul* Soul);
	virtual void AddGold(class ATreasure* Treasure);
};
