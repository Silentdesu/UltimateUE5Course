#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Treasure.generated.h"

UCLASS()
class ULTIMATEUE5COURSE_API ATreasure : public AItem
{
	GENERATED_BODY()

public:

	FORCEINLINE
	int32 GetGold() const { return Gold; }
	
protected:
	
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                  const FHitResult& SweepResult) override;

private:

	UPROPERTY(EditAnywhere, Category = "Economy")
	int32 Gold;
};
