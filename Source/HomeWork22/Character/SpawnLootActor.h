// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FunctionLibrary/Types.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/ArrowComponent.h"
#include "SpawnLootActor.generated.h"

UCLASS()
class HOMEWORK22_API ASpawnLootActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnLootActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USphereComponent* CollisionSphere = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USkeletalMeshComponent* SkeletalMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UStaticMeshComponent* StaticMeshComp = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UArrowComponent* ArrowSpawnComp = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UParticleSystemComponent* ParticleForCaseActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UParticleSystemComponent* ParticleFx = nullptr;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* AnimSpawnLoot= nullptr;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Actor to Spawn")
	TSubclassOf<class AActor> WeaponSpawn = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo Actor to Spawn")
	TSubclassOf<class AActor> AmmoSpawn = nullptr;

	UFUNCTION(BlueprintCallable)
		void OnOpenSuccess();

	UFUNCTION(BlueprintCallable)
	void GetWeaponInfoByName(FName WeaponName, FDropItem& _WeaponDrop);

	UFUNCTION(BlueprintCallable)
	FDropItem GetRandomItemToDrop(ERariness ItemRariness);

	UFUNCTION(BlueprintCallable)
	FLootCase GetRandomCase();



	UPROPERTY(EditAnywhere, BlueprintreadWrite)
	int32 CountOfAmmoToDrop = 1;

	UPROPERTY(EditAnywhere, BlueprintreadWrite)
		bool RandomCase = false;

	UPROPERTY(EditAnywhere, BlueprintreadWrite)
		ERariness Rariness = ERariness::Common_Type;

};
