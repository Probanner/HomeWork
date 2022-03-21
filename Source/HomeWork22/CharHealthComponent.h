// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HeathComponent.h"
#include "CharHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShieldChange,float, ShieldValue, float, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldBroke);

UCLASS()
class HOMEWORK22_API UCharHealthComponent : public UHeathComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Shield")
	FOnShieldChange OnShieldChange;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Shield")
	FOnShieldBroke OnShieldBroke;

	FTimerHandle CoolDownShieldTimer;
	FTimerHandle CoolDownRecoveryShieldTimer;

protected:

float CharShield = 100.0f;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float CoolDownShielRecoveryTime = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float ShieldRecoverRate = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float ShieldRecoverValue = 1.0f;



	void ChangeCurrentHealth(float ChangeValue) override;
	UFUNCTION(BlueprintCallable)
	void ChangeShieldValue(float ChangeShieldValue);

	UFUNCTION(BlueprintCallable)
	float GetCurrentShield();

	void CoolDownShieldEnd();

	void RecoveryShield();

	//NetWork
	UFUNCTION(NetMulticast, Reliable)
	void OnShieldChange_Multicast(float ShieldValue, float Damage);

	UFUNCTION(NetMulticast, Reliable)
	void ValidateShieldTimes_Multicast(FTimerHandle NetCoolDownShieldTimer);
};
