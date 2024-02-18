#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Hitable.h"
#include "BreakableProp.generated.h"

class UGeometryCollectionComponent;

UCLASS()
class ULTIMATEUE5COURSE_API ABreakableProp : public AActor, public IHitable
{
	GENERATED_BODY()
	
public:	
	ABreakableProp();
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere)
	UGeometryCollectionComponent* GeometryCollectionComponent;
};
