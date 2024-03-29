#include "Items/Pickups/Soul.h"

#include "Interfaces/PickUp.h"

void ASoul::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IPickUp* PickUpInterface = Cast<IPickUp>(OtherActor))
	{
		PickUpInterface->AddSoul(this);
		SpawnPickUpVFX();
		SpawnPickUpSFX();
		Destroy();
	}
}
