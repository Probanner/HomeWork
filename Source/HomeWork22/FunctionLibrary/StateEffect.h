// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Particles/ParticleSystemComponent.h"
#include "StateEffect.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class HOMEWORK22_API UStateEffect : public UObject
{
	GENERATED_BODY()
public:
	virtual bool InitObject(AActor* Actor, FName ActorBoneName);
	virtual void ExecuteEffect(float DeltaTime);
	virtual void DestroyObject();

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "PossibleInteractSurface")
	TArray<TEnumAsByte<EPhysicalSurface>> PossibleInteractSurface;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CanBeStacked")
	bool CanBeStacked = false;

	AActor* myActor = nullptr;
};

UCLASS()
class HOMEWORK22_API UStateEffect_ExecuteOnce: public UStateEffect
{
	GENERATED_BODY()
	public:
	bool InitObject(AActor* Actor, FName ActorBoneName) override;
	void DestroyObject() override;

	virtual void ExecuteOnce();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Execute Once")
	float Power = 20.0f;

};

UCLASS()
class HOMEWORK22_API UStateEffect_ExecuteTimer : public UStateEffect
{
	GENERATED_BODY()
public:


	

	bool InitObject(AActor* Actor, FName ActorBoneName) override;
	void DestroyObject() override;

	virtual void ExecuteByTimer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer Effect")
		float Power = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer Effect")
		float Timer = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer Effect")
		float TimerRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer Effect")
		UParticleSystem* ParticleForEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer Effect")
		FName BoneToAttach;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer Effect")
	bool EffectScaleDecreaseAtEnd = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer Effect")
	bool EffectScaleIncreaseAtEnd = false;

	
	FTimerHandle StateEffect_ExecuteTimer;
	FTimerHandle StateEffect_EffectTimer;
	FTimerHandle TimerEffectFade;

	float EffectFadeRate = 0.01f;
	float Secs = 0.0f;
	float ScaleOfScaleVector = 0.0f;
	int32 TimerDoneTimes = 0;

	UParticleSystemComponent* ParticleEmitter = nullptr;
	float TimeBetween = 0.0f;
	float TimeToDegree = 0.0f;
	void RefreshTimerDamageTick();

	void EffectScaleDecrease();

};