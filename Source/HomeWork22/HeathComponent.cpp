// Fill out your copyright notice in the Description page of Project Settings.


#include "HeathComponent.h"

// Sets default values for this component's properties
UHeathComponent::UHeathComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHeathComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHeathComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UHeathComponent::GetCurrentHealth()
{
	return Health;
}

void UHeathComponent::SetCurrentHeath(float _health)
{
	Health = _health;
}

void UHeathComponent::ChangeCurrentHealth(float ChangeValue)
{
	ChangeValue*=DamageCoef;
	Health += ChangeValue;

	if (Health > 100.0f)
	{
		Health = 100.0f;
	
	}
	else if(Health <= 0)
	{
		
		Health = 0.0f;
		if (bIsAlive)
		{
			OnHeathChange.Broadcast(Health, ChangeValue);
			OnDead.Broadcast();
			bIsAlive = false;
		}
	
	}
	if (Health>=0.5f)
	{
		OnHeathChange.Broadcast(Health, ChangeValue);
	}
	
}

void UHeathComponent::DeadEvent_Implementation()
{
	//BP
}

