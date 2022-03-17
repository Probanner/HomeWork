// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/SpawnLootActor.h"
#include "UHomeWork22GameInstance.h"


// Sets default values
ASpawnLootActor::ASpawnLootActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	RootComponent = CollisionSphere;
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Component"));
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));

	StaticMeshComp->SetGenerateOverlapEvents(true);
	StaticMeshComp->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshComp->SetupAttachment(RootComponent);

	ParticleForCaseActor = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle For Case"));


	ArrowSpawnComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow Weapon Component"));
	ArrowSpawnComp->SetupAttachment(CollisionSphere);


	ParticleFx = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle System"));
	ParticleFx->SetupAttachment(CollisionSphere);
}

// Called when the game starts or when spawned
void ASpawnLootActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (SkeletalMesh && !SkeletalMesh->SkeletalMesh)
	{
		SkeletalMesh->DestroyComponent(true);
	}
	if (StaticMeshComp && !StaticMeshComp->GetStaticMesh())
	{
		StaticMeshComp->DestroyComponent(true);
	}
	if (!ParticleForCaseActor->Template)
	{
		ParticleForCaseActor->DestroyComponent();
	}
}

// Called every frame
void ASpawnLootActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnLootActor::OnOpenSuccess()
{
	this->Destroy();
}

void ASpawnLootActor::GetWeaponInfoByName(FName WeaponName, FDropItem& _WeaponDrop)
{
	FDropItem DropItemTemp;
	FWeaponSlot WeaponSlotTemp;
	UHomeWork22GameInstance* MyGI = Cast<UHomeWork22GameInstance>(GetGameInstance());
}

FDropItem ASpawnLootActor::GetRandomItemToDrop(ERariness ItemRariness)
{
	FDropItem DropItemToDrop;
	UHomeWork22GameInstance* MyGI = Cast<UHomeWork22GameInstance>(GetGameInstance());

	
	TArray<FDropItem> ArrayOfItems = MyGI->GetAllDropItemsByRariness(ItemRariness);
	if (ArrayOfItems.Num()!=0)
	{
		//возвращает рандомный индекс элемента, а затема делает return элемента массива с этим индексом
		return DropItemToDrop = ArrayOfItems[FMath::RandRange(0, ArrayOfItems.Num()-1)];
	}
	return DropItemToDrop;
}

FLootCase ASpawnLootActor::GetRandomCase()
{
	UHomeWork22GameInstance* MyGI = Cast<UHomeWork22GameInstance>(GetGameInstance());
	return MyGI->GetRandomCase();
}

