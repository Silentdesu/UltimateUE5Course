#include "Items/Item.h"
#include "DrawDebugHelpers.h"
#include "Characters/CourseCharacter.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(300.0F);
	SphereComponent->SetupAttachment(GetRootComponent());

	EmbersEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EmbersEffect"));
	EmbersEffect->SetupAttachment(GetRootComponent());
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	if (State == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.0F, 0.0F, TransformSin(DeltaTime)));
	}
}

void AItem::Equip(USceneComponent* Parent, FName InSocketName)
{
}

void AItem::AttachMeshToSocket(USceneComponent* Parent, FName InSocketName)
{
	const FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget,
	                                      EAttachmentRule::SnapToTarget, true);
	this->AttachToComponent(Parent, Rules, InSocketName);
}

void AItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
	if (ACourseCharacter* Character = Cast<ACourseCharacter>(OtherActor))
	{
		Character->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACourseCharacter* Character = Cast<ACourseCharacter>(OtherActor))
	{
		Character->SetOverlappingItem(nullptr);
	}
}

float AItem::TransformSin(const float& Value) const
{
	return Amplitude * FMath::Sin(Value * TimeConstants);
}
