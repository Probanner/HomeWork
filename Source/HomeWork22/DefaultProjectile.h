// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "FunctionLibrary/Types.h"
#include "DefaultProjectile.generated.h"



UCLASS()
class HOMEWORK22_API ADefaultProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADefaultProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UStaticMeshComponent* BulletMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USphereComponent* BulletCollisionSphere = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UProjectileMovementComponent* BulletProjectileMovement = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UParticleSystemComponent* BulletFx = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "ProjectileInfo")
		FProjectileInfo ProjectileSetting;


	FVector SpawnLocation;

	float DamageTemp = ProjectileSetting.ProjectileDamage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
		void BulletCollisionSphereHit(class UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
		void BulletCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void BulletCollisionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintNativeEvent)
		void RenderTargetPrint(class UPrimitiveComponent* HitComp,
			AActor* OtherActor, UPrimitiveComponent* OtherComponent,
			FVector NormalImpulse, const FHitResult& Hit, FVector2D HittedUV);

	FVector2D FindCollisionUVOnSkeletalMesh(const struct FHitResult& Hit);
	
	void InitProjectile(FProjectileInfo InitParam, FVector Direction);

	UFUNCTION(NetMulticast, Reliable)
	void SpeedInit_Multicast(FVector Direction, float Speed);

	UFUNCTION(NetMulticast, Reliable)
	void SpawnHitDecal_Multicast(UMaterialInterface* DecalMaterial, UPrimitiveComponent* OtherComponent, FHitResult Hit, bool bIsLikeBomb);

	UFUNCTION(NetMulticast, Reliable)
	void SpawnHitFX_Multicast(UParticleSystem* HitParticle, FHitResult Hit);

	UFUNCTION(NetMulticast, Reliable)
	void SpawnHitSound_Multicast(USoundBase* HitSound, FHitResult Hit);

};
