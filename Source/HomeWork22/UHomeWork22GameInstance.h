// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FunctionLibrary/Types.h"
#include "Engine/DataTable.h"
#include "DefaultWeapon.h"
#include "UHomeWork22GameInstance.generated.h"


UCLASS()
class HOMEWORK22_API UHomeWork22GameInstance: public UGameInstance
{
	GENERATED_BODY()
		

public:

	//table
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category ="WeaponSetting")
	UDataTable* WeaponInfoTable = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropItem")
	UDataTable* DropItemInfoTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LootCasesTable")
	UDataTable* LootCasesTable = nullptr;

	UFUNCTION(BlueprintCallable)
	bool GetWeaponInfoByName(FName NameWeapon, FWeaponInfo& OutInfo, bool &withPistol);


	UFUNCTION(BlueprintCallable)
	bool GetDropItemInfoByName(FName NameItem, FDropItem& ItemToDrop);


	UFUNCTION(BlueprintCallable)
	TArray <FDropItem> GetAllDropItemsByRariness(ERariness ItemRariness);

	UFUNCTION(BlueprintCallable)
		FLootCase GetRandomCase();

	UFUNCTION(BlueprintCallable)
	TSubclassOf<class APickupActor> GetActorToDropByName(FName ActorDropName, bool bIsWeapon);

	UFUNCTION(BlueprintCallable)
	TSubclassOf<class APickupActor> GetRandomActorToDrop(int32 CommonChance, int32 RareChance, int32 EpicChance, int32 LegendaryChance);

	UFUNCTION(BlueprintCallable)
	TSubclassOf<class APickupActor> GetRandomItemToDropByRariness(ERariness DropRariness);
};


