// Fill out your copyright notice in the Description page of Project Settings.


#include "UHomeWork22GameInstance.h"


bool UHomeWork22GameInstance::GetWeaponInfoByName(FName NameWeapon, FWeaponInfo& OutInfo, bool &withPistol)
{
	bool bIsFind = false;
	FWeaponInfo* WeaponInfoRow;
	if (WeaponInfoTable)
	{
		WeaponInfoRow = WeaponInfoTable->FindRow<FWeaponInfo>(NameWeapon, "", false);
		if (WeaponInfoRow)
		{
			bIsFind = true;
			OutInfo = *WeaponInfoRow;
			if (WeaponInfoRow->WeaponType == EWeaponType::PistolType)
			{
				withPistol = true;
			}
			else
			{
				withPistol = false;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UHomeWork22Instance::GetWeaponInfoByName - WeaponTable -NULL"));
	}
	return bIsFind;

}

bool UHomeWork22GameInstance::GetDropItemInfoByName(FName NameItem, FDropItem& ItemToDrop)
{

	if (DropItemInfoTable)
	{
		FDropItem* DropItemInfoRow;
		TArray<FName> ArrayOfRowNames = DropItemInfoTable->GetRowNames();

		for (int i = 0; i < ArrayOfRowNames.Num(); i++)
		{
			DropItemInfoRow = DropItemInfoTable->FindRow<FDropItem>(ArrayOfRowNames[i],"");

			//проверка на совпадение
			if (DropItemInfoRow->WeaponName.ToString() == NameItem.ToString())
			{
				
				ItemToDrop = *DropItemInfoRow;

				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("DropItemInfoRow->WeaponName.ToString() == NameItem.ToString():   TRUE"));
				}

				return true;
			}
		}


	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UHomeWork22Instance::GetDropItemInfoByName - DropItemInfoTable -NULL"));
	}
	return false;
}

//Найти все предметы, согласно редкости
TArray<FDropItem> UHomeWork22GameInstance::GetAllDropItemsByRariness(ERariness ItemRariness)
{
	TArray<FDropItem> ArrayToReturn;
	if (DropItemInfoTable)
	{
		FDropItem* DropItemInfoRow;
		TArray<FName> ArrayOfRowNames = DropItemInfoTable->GetRowNames();

		for (int i = 0; i< ArrayOfRowNames.Num();i++)
		{
			DropItemInfoRow = DropItemInfoTable->FindRow<FDropItem>(ArrayOfRowNames[i], "");

				if (DropItemInfoRow->LootRariness == ItemRariness)
				{
					ArrayToReturn.Add(*DropItemInfoRow);
				}	
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UHomeWork22Instance::GetDropItemInfoByName - DropItemInfoTable -NULL"));
	}
	return ArrayToReturn;

}

FLootCase UHomeWork22GameInstance::GetRandomCase()
{
	FLootCase DropItemInfoRow;
	if (LootCasesTable)
	{
	
		TArray<FName> ArrayOfRowNames = LootCasesTable->GetRowNames();
		DropItemInfoRow = *LootCasesTable->FindRow<FLootCase>(ArrayOfRowNames[FMath::RandRange(0, ArrayOfRowNames.Num() - 1)],"");
		return DropItemInfoRow;


	}
	return DropItemInfoRow;
}

TSubclassOf<class APickupActor> UHomeWork22GameInstance::GetActorToDropByName(FName ActorDropName, bool bIsWeapon)
{
	if (DropItemInfoTable && !bIsWeapon)
	{
		TArray<FName> ArrayOfRowNames = DropItemInfoTable->GetRowNames();
		for (int i = 0; i < ArrayOfRowNames.Num();i++)
		{
			FDropItem ItemToDrop = *DropItemInfoTable->FindRow<FDropItem>(ArrayOfRowNames[i], "");
			if (ItemToDrop.WeaponName == ActorDropName)
			{
				return ItemToDrop.ActorToDrop;
			}
		}
	}
	else if (DropItemInfoTable && bIsWeapon)
	{
		TArray<FName> ArrayOfRowNames = DropItemInfoTable->GetRowNames();
		for (int i = 0; i < ArrayOfRowNames.Num(); i++)
		{
			FDropItem ItemToDrop = *DropItemInfoTable->FindRow<FDropItem>(ArrayOfRowNames[i], "");
			if (ItemToDrop.WeaponName == ActorDropName && ItemToDrop.bIsAWeapon)
			{
				return ItemToDrop.ActorToDrop;
			}
		}
	}

	return NULL;
}


TSubclassOf<class APickupActor> UHomeWork22GameInstance::GetRandomActorToDrop(int32 CommonChance, int32 RareChance, int32 EpicChance, int32 LegendaryChance)
{

	if (DropItemInfoTable)
	{
		int32 RandTemp = FMath::RandRange(1,100);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Orange, TEXT("Random = ") + FString::FromInt(RandTemp));
		}

		if (RandTemp <= CommonChance)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Red, TEXT("Common_Type"));
			}
			return GetRandomItemToDropByRariness(ERariness::Common_Type);
		}
		if(RandTemp > CommonChance && RandTemp <=(CommonChance+RareChance))
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Red, TEXT("Rare_Type"));
			}
			return GetRandomItemToDropByRariness(ERariness::Rare_Type);
		}
		 if (RandTemp > (CommonChance + RareChance) && RandTemp <= (CommonChance + RareChance + EpicChance))
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Red, TEXT("Epic_Type"));
			}
			return GetRandomItemToDropByRariness(ERariness::Epic_Type);
		}
		if (RandTemp > (CommonChance + RareChance + EpicChance) && RandTemp <= (CommonChance + RareChance + EpicChance + LegendaryChance))
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Red, TEXT("Legendary_Type"));
			}
			return GetRandomItemToDropByRariness(ERariness::Legendary_Type);
		}

		
	}
	return NULL;
}

// Рандомный предмет по редкости
TSubclassOf<class APickupActor> UHomeWork22GameInstance::GetRandomItemToDropByRariness(ERariness DropRariness)
{
	TArray<FDropItem> ArrayOfItemsByRareness = GetAllDropItemsByRariness(DropRariness);

	if (ArrayOfItemsByRareness.Num() > 0)
	{
		return ArrayOfItemsByRareness[FMath::RandRange(0, ArrayOfItemsByRareness.Num() - 1)].ActorToDrop;
	}
	else
	{
		return NULL;
	}
	return NULL;
}

