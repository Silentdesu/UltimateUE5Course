#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;
class USoundBase;
class UNiagaraComponent;
class UNiagaraSystem;

enum class EItemState : int8
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class ULTIMATEUE5COURSE_API AItem : public AActor
{
	GENERATED_BODY()

public:
	AItem();

	virtual void Tick(float DeltaTime) override;
	void AttachMeshToSocket(USceneComponent* Parent, FName InSocketName);

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Category= "Sin Functions", BlueprintPure)
	float TransformSin(const float& Value) const;

	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                          UPrimitiveComponent* OtherComp,
	                          int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SpawnPickUpVFX() const;
	void SpawnPickUpSFX() const;
	void SpawnEquipSFX() const;
	
protected:

	UPROPERTY(Category= "Sine Parameters", EditAnywhere, BlueprintReadWrite)
	float Amplitude = 0.25F;

	UPROPERTY(Category= "Sine Parameters", EditAnywhere, BlueprintReadWrite)
	float TimeConstants = 5.0F;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* Holder;

	UPROPERTY(EditAnywhere, Category = VFX)
	UNiagaraComponent* ItemEffect;

	EItemState State = EItemState::EIS_Hovering;

private:

	UPROPERTY(EditAnywhere, Category = VFX)
	UNiagaraSystem* PickUpVFX;

	UPROPERTY(EditDefaultsOnly, Category = SFX)
	USoundBase* EquipSFX;

	UPROPERTY(EditDefaultsOnly, Category = SFX)
	USoundBase* PickUpSFX;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;
};
