#include "Items/Pickups/Treasure.h"
#include "Characters/CourseCharacter.h"

void ATreasure::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IPickUp* Character = Cast<IPickUp>(OtherActor))
	{
		Character->AddGold(this);
		SpawnPickUpVFX();
		SpawnPickUpSFX();
		Destroy();
	}
}
