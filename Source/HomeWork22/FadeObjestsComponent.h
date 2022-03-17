// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Components/ActorComponent.h"


//My Includes
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "CollisionQueryParams.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstance.h"
#include "UObject/UObjectArray.h"


#include "FadeObjestsComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOMEWORK22_API UFadeObjestsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	UFadeObjestsComponent();


	UPROPERTY(EditAnywhere, Category = "Enable Fade")
	bool FadeEnabled = true;

	UPROPERTY(EditAnywhere, Category = "Capsule Radius")
		float CapsuleRadius = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Capsule fade speed")
		float fadeSpeed = 1.0f;

	//Actors to ignore
	UPROPERTY(EditAnywhere, Category = "Array To Ignore")
		TArray<TSubclassOf<AActor>> ToIgnore;

	UPROPERTY(EditAnywhere, Category = "Trace Channel to work with")
		TEnumAsByte<ECollisionChannel> WorkChannel;

	UPROPERTY(EditAnywhere, Category = "Fade Material")
		UMaterialInstance* FadeMaterial;

	UPROPERTY(EditAnywhere, Category = "Draw Debug Capsule")
	bool DebugCapsule = false;

	//ETraceTypeQuery MyTraceType;

	FCollisionQueryParams myParams;

	// Массив получаемый капсулой
	TArray<UPrimitiveComponent*> CapsulePrimitiveArray;
	//TArray<UMaterialInterface*> CapsuleMaterialArray;

	// массив - запоминалка
	TArray<UPrimitiveComponent*> PrimitiveArrayToCompare;
	TArray<UMaterialInterface*> MaterialArrayToCompare;

	//Временные массивы
	TArray<UPrimitiveComponent*> PrimitiveArrayTemp;
	TArray<UMaterialInterface*> MaterialArrayTemp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddObjectsToFade();
	void MakeFadeOrNot(bool AllAppeares);

private:
	// create tarray for hit results
	TArray<FHitResult> OutHits;

};
