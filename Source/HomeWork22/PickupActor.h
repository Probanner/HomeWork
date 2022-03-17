// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "WeaponSwitchActor.h"
#include "PickupActor.generated.h"

UCLASS()
class HOMEWORK22_API APickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupActor();

	UPROPERTY(BlueprintReadOnly, Category = Components, VisibleAnywhere)
	USphereComponent* CollisionSphere;
	UPROPERTY(BlueprintReadOnly, Category = Components, VisibleAnywhere)
	USkeletalMeshComponent* SkeletalMesh;
	UPROPERTY(BlueprintReadOnly, Category = Components, VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;


	



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void CollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 BodyIndex, bool bFromSweep, const FHitResult& HitResult);

	UFUNCTION(BlueprintImplementableEvent)
	void CharOverlap(UWeaponSwitchActor* ChatInventoryComponent);

};
