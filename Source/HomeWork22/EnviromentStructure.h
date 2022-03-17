// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FunctionLibrary/StateEffect.h"
#include "DestructibleComponent.h"
#include "FunctionLibrary/IGameActor.h"
#include "EnviromentStructure.generated.h"

UCLASS()
class HOMEWORK22_API AEnviromentStructure : public AActor, public IIGameActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnviromentStructure();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	EPhysicalSurface GetSurfaceType() override;
	//Effect
	virtual TArray<UStateEffect*> GetAllCurrentEffects();
	void RemoveEffect(UStateEffect* RemoveEffect) override;
	void AddEffect(UStateEffect* NewEffect) override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<UStateEffect*> Effects;

	UPROPERTY(EditAnywhere)
	bool bDrawDebug = false;

	bool bIsExploded = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UDestructibleComponent* DestructibleMesh = nullptr;

	UFUNCTION(BlueprintCallable)
	void Explosion(float BaseDamageValue, float Radius, UParticleSystem* ExplosionParticle, TSubclassOf<UStateEffect> ExpEffect, USoundBase* ExplosionSound);

	FTimerHandle myTimerToDestroy;

	void DestroyActor1();

};
