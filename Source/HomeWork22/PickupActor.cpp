// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupActor.h"
#include "Character/HomeWork22Character.h"

// Sets default values
APickupActor::APickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	RootComponent = CollisionSphere;

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APickupActor::CollisionSphereBeginOverlap);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMesh->SetupAttachment(RootComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent1"));
	MeshComponent->SetupAttachment(RootComponent);

	SetReplicates(true);

}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickupActor::CollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 BodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	AHomeWork22Character* OverlappedChar = Cast<AHomeWork22Character>(OtherActor);
	if (OverlappedChar)
	{
		UWeaponSwitchActor* InventoryComp = Cast<UWeaponSwitchActor>(OverlappedChar->GetComponentByClass(UWeaponSwitchActor::StaticClass()));
		if (InventoryComp)
		{
			CharOverlap_Client(InventoryComp);
			//CharOverlap(InventoryComp);
		}
	}
}

void APickupActor::CharOverlap_Client_Implementation(UWeaponSwitchActor* ChatInventoryComponent)
{
	CharOverlap(ChatInventoryComponent);
}

