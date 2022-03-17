// Fill out your copyright notice in the Description page of Project Settings.


#include "MagazineActor.h"

// Sets default values
AMagazineActor::AMagazineActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;


	StaticMeshMag = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshMag->SetupAttachment(RootComponent);
	StaticMeshMag->SetCanEverAffectNavigation(false);

	BulletFx = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	BulletFx->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AMagazineActor::BeginPlay()
{
	Super::BeginPlay();
	this->SetLifeSpan(5.0f);
}

// Called every frame
void AMagazineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

