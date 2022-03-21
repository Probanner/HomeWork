// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HeathComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChange, float, Health, float, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDead);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOMEWORK22_API UHeathComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHeathComponent();
	UPROPERTY(BlueprintAssignable,EditAnywhere,BlueprintReadWrite, Category = "Health")
	FOnHealthChange OnHeathChange;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Health")
	FOnDead OnDead;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	float Health = 100.0;

public:	
	// Called every frame

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float DamageCoef = 1.0f;


	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetCurrentHealth();

	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetCurrentHeath(float _health);

	UFUNCTION(BlueprintCallable, Category = "Health")
    virtual	void ChangeCurrentHealth(float ChangeValue);

	UFUNCTION(BlueprintNativeEvent)
	void DeadEvent();

	bool bIsAlive = true;

	//NetWork
	UFUNCTION(NetMulticast, Reliable)
	void OnHeathChange_Multicast(float NetHealth, float NetDamage);

	UFUNCTION(NetMulticast, Reliable)
	void OnDead_Multicast();

};
