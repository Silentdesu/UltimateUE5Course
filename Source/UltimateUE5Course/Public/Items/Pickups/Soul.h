#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

UCLASS()
class ULTIMATEUE5COURSE_API ASoul : public AItem
{
	GENERATED_BODY()

public:

	FORCEINLINE
	int32 GetSouls() const { return Souls; }
	
protected:
	
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                  const FHitResult& SweepResult) override;

private:

	UPROPERTY(EditAnywhere)
	int32 Souls;
};
