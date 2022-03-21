// Fill out your copyright notice in the Description page of Project Settings.


#include "CharHealthComponent.h"

void UCharHealthComponent::ChangeCurrentHealth(float ChangeValue)
{
	float CurrentDamage = ChangeValue * DamageCoef;

	if (Health < 100.0f && ChangeValue>0)
	{
		Super::ChangeCurrentHealth(ChangeValue);
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, TEXT("Health < 100.0f && ChangeValue>0"));
	}
	else if (CharShield > 0.0f)
	{
		ChangeShieldValue(CurrentDamage);
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, TEXT("(CharShield > 0.0f)"));
		if (CharShield < 0.0f)
		{
			//FX
		}
	
	}//MedicKit
	else //Damage to Health
	{
		Super::ChangeCurrentHealth(ChangeValue);
		//GEngine->AddOnScreenDebugMessage(-1,10,FColor::Green, TEXT("Super::ChangeCurrentHealth(ChangeValue)"));
	}	
	// for char
}


void UCharHealthComponent::ChangeShieldValue(float ChangeShieldValue)
{
	CharShield+=ChangeShieldValue;
	if (CharShield > 100.0f)
	{
		CharShield = 100.0f;
	}
	else if (CharShield < 0)
	{
		CharShield = 0.0f;
	}


	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(CoolDownShieldTimer,this,&UCharHealthComponent::CoolDownShieldEnd, CoolDownShielRecoveryTime,false);
		//Обновляем таймер только после получения дамага
		if (ChangeShieldValue < 0)
		{
			ValidateShieldTimes_Multicast(CoolDownRecoveryShieldTimer);
			//GetWorld()->GetTimerManager().ClearTimer(CoolDownRecoveryShieldTimer);
		}
	
	}
	OnShieldChange_Multicast(CharShield, ChangeShieldValue);
	//OnShieldChange.Broadcast(CharShield, ChangeShieldValue);
}

float UCharHealthComponent::GetCurrentShield()
{
	return CharShield;
}

void UCharHealthComponent::CoolDownShieldEnd()
{
	if (GetWorld() && Health>0)
	{
		GetWorld()->GetTimerManager().SetTimer(CoolDownRecoveryShieldTimer, this, &UCharHealthComponent::RecoveryShield, ShieldRecoverRate, true);
	}
}

void UCharHealthComponent::RecoveryShield()
{
	float ShieldTmp = CharShield;
	ShieldTmp +=ShieldRecoverValue;
	if (ShieldTmp >= 100.0f)
	{
		CharShield = 100.0f;
		ValidateShieldTimes_Multicast(CoolDownRecoveryShieldTimer);
		//GetWorld()->GetTimerManager().ClearTimer(CoolDownRecoveryShieldTimer);
	}
	else
	{
		CharShield = ShieldTmp;
	}
	OnShieldChange_Multicast(CharShield, ShieldRecoverValue);
	//OnShieldChange.Broadcast(CharShield, ShieldRecoverValue);

}

void UCharHealthComponent::OnShieldChange_Multicast_Implementation(float ShieldValue, float Damage)
{
	OnShieldChange.Broadcast(ShieldValue, Damage);
}

void UCharHealthComponent::ValidateShieldTimes_Multicast_Implementation(FTimerHandle NetCoolDownShieldTimer)
{
	GetWorld()->GetTimerManager().ClearTimer(NetCoolDownShieldTimer);
}

