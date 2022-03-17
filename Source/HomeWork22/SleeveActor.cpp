// Fill out your copyright notice in the Description page of Project Settings.

#include "SleeveActor.h"


// Sets default values
ASleeveActor::ASleeveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	StaticMeshSleeve = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshSleeve->SetGenerateOverlapEvents(false);
	StaticMeshSleeve->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshSleeve->SetupAttachment(RootComponent);


	SleeveFx = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BulletFxParticleSystemComponent"));
	SleeveFx->SetupAttachment(RootComponent);

	StaticMeshSleeve->SetMobility(EComponentMobility::Movable);
	StaticMeshSleeve->SetSimulatePhysics(true);
	StaticMeshSleeve->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);


}

// Called when the game starts or when spawned
void ASleeveActor::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle myTimer;
	this->SetLifeSpan(5.0f);

}

// Called every frame
void ASleeveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASleeveActor::SleeveInit() 
{

	StaticMeshSleeve->AddImpulse((FVector(0, 1, 0) * 500));

}
