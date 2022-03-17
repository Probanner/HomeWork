// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FunctionLibrary/StateEffect.h"
#include "FunctionLibrary/Types.h"
#include "IGameActor.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIGameActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HOMEWORK22_API IIGameActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	

   /* UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interface")
	void AvailableForEffectsBP();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	bool AvailableForEffects();
	*/

	virtual EPhysicalSurface GetSurfaceType();

	virtual TArray<UStateEffect*> GetAllCurrentEffects();
	virtual void RemoveEffect(UStateEffect* RemoveEffect);
	virtual void AddEffect(UStateEffect* NewEffect);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DropWeaponItem(FDropItem WeaponToDrop);

};
