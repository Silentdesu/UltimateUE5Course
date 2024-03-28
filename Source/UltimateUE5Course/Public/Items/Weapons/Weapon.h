#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "UltimateUE5Course/Constants.h"
#include "Weapon.generated.h"

class UBoxComponent;

UCLASS()
class ULTIMATEUE5COURSE_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UBoxComponent* GetBoxComponent() const { return BoxComponent; }

public:

	TArray<AActor*> IgnoreActors;
	void Equip(USceneComponent* Parent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

private:

	void BoxTrace(FHitResult& HitResult);
	void ExecuteGetHit(const FHitResult& HitResult) const;
	
	UPROPERTY(VisibleDefaultsOnly)
	UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	UPROPERTY(EditAnywhere)
	bool bShowBoxDebug;
	
	UPROPERTY(EditAnywhere)
	FVector BoxHalfExtentSize;

	UPROPERTY(EditAnywhere)
	float Damage = 20.0F;

	
	FORCEINLINE bool IsAlly(const AActor* OtherActor) const
	{ return GetOwner()->ActorHasTag(ENEMY_TAG) && OtherActor->ActorHasTag(ENEMY_TAG); }
};
