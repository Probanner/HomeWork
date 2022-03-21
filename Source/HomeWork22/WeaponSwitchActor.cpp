// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSwitchActor.h"
#include "Character/HomeWork22Character.h"
#include "FunctionLibrary/IGameActor.h"
#include "Net/UnrealNetwork.h"
#include "DefaultWeapon.h"

// Sets default values for this component's properties
UWeaponSwitchActor::UWeaponSwitchActor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	this->SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UWeaponSwitchActor::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UWeaponSwitchActor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponSwitchActor::SwitchWeaponToIndex(int32 ChangeToIndex)
{
	if (WeaponSlots.IsValidIndex(ChangeToIndex))
	{

		if (WeaponSlots.Num() - 1 < WeaponIndexToDrop)
		{
			WeaponIndexToDrop = WeaponSlots.Num() - 1;
		}
		

		//WeaponSlots[WeaponIndexToDrop].AdditionalInfo.Round = OldInfo.Round;
		if (ChangeToIndex != CurrentIndex)
		{
			WeaponIndexToDrop = CurrentIndex;
		}
			
		OnSwitchWeapon_Server(WeaponSlots[ChangeToIndex]);
		//OnSwitchWeapon.Broadcast(WeaponSlots[ChangeToIndex]);
	
	}
	else
	{
		OnSwitchWeapon_Server(WeaponSlots[0]);
		
		//OnSwitchWeapon.Broadcast(WeaponSlots[0]);
	}
	CurrentIndex = ChangeToIndex;
	
}

//Находит индекс оружия по имени
int32 UWeaponSwitchActor::GetWeaponIndexSlotByName(FName IdWeaponName)
 {
	int32 FoundedId = 0;
	for (int i = 0; i < WeaponSlots.Num(); i++)
	{
		if (WeaponSlots[i].ItemName == IdWeaponName)
			FoundedId = i;
			
	}
	return FoundedId;
}

bool UWeaponSwitchActor::CheckAmmoForWeapon(EWeaponType _TypeWeapon)
{
	for (int i = 0; i<AmmoSlots.Num();i++)
	{
		if (AmmoSlots[i].WeaponType == _TypeWeapon && AmmoSlots[i].Count>0)
		{	
			return true;
		}
	}
	OnWeaponAmmoEmpty_Multicast(_TypeWeapon);
	//OnWeaponAmmoEmpty.Broadcast(_TypeWeapon);

	return false;
}


void UWeaponSwitchActor::OnWeaponAmmoEmpty_Multicast_Implementation(EWeaponType DWeaponType)
{
	OnWeaponAmmoEmpty.Broadcast(DWeaponType);
}




void UWeaponSwitchActor::FOnFireInInventory()
{
	OnFireInventory_Multicast(CurrentIndex);
	//OnFireInventory.Broadcast(GetCurrentIndex());
}



void UWeaponSwitchActor::OnFireInventory_Multicast_Implementation(int32 CurrentWeaponIndexInventory)
{
	OnFireInventory.Broadcast(CurrentWeaponIndexInventory);
}

void UWeaponSwitchActor::ReloadInInventory()
{
	OnReloadInInventory_Multicast(CurrentIndex);
	//OnReloadInInventory.Broadcast(GetCurrentIndex());
}

void UWeaponSwitchActor::OnReloadInInventory_Multicast_Implementation(int32 CurrentWeaponIndexInventory)
{
	OnReloadInInventory.Broadcast(CurrentWeaponIndexInventory);
}



int32 UWeaponSwitchActor::ReturnLastingAmmo(EWeaponType _TypeWeapon)
{

	for (int i = 0; i < AmmoSlots.Num(); i++)
	{
		if (AmmoSlots[i].WeaponType == _TypeWeapon )
		{
			return AmmoSlots[i].Count;
		}
	}

	return 0;
}

bool UWeaponSwitchActor::CheckCanTakeAmmo(EWeaponType AmmoType)
{
	bool result = false;
	for (int i = 0; i< AmmoSlots.Num();i++) 
	{
		if (AmmoSlots[i].WeaponType == AmmoType)
		{
			if (AmmoSlots[i].Count < AmmoSlots[i].MaxCount)
			{
				result = true;
			}
			else
			{
				return false;
			}
		}
	}
	if (!result)
	{
		FAmmoSlot NewSlot;
		NewSlot.Count = 0;
		NewSlot.MaxCount = 100;
		NewSlot.WeaponType = AmmoType;
		AmmoSlots.Add(NewSlot);
		result = true;
	}

	return result;
}

//Проверка на свободный слот


void UWeaponSwitchActor::SaveItemToInventory()
{

}

//Замена текущего оружия, оружием на полу
void UWeaponSwitchActor::SwitchWeaponToInventory(FWeaponSlot NewWeapon, int32 IndexOfSlot)
{
	
	if (WeaponSlots.IsValidIndex(IndexOfSlot) /* && DropCurrentWeaponFromInventory(IndexOfSlot)*/)
	{
		WeaponSlots[IndexOfSlot] = NewWeapon;
		OnSwitchWeapon_Server(NewWeapon);
		//OnSwitchWeapon.Broadcast(NewWeapon);
	}

}


bool UWeaponSwitchActor::DropCurrentWeaponFromInventory(ADefaultWeapon* CurrentWeapon)
{
	if (WeaponSlots.Num()>1)
	{
		FDropItem WeaponToDrop;

		TArray <AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(),CurrentWeapon->WeaponSetting.WeaponClass,OutActors);
		if (OutActors.Num()>0)
		{
			// Добавляем в структуру скелет оружия
			USkeletalMeshComponent* SkeletalMeshForWeaponToDrop = Cast<USkeletalMeshComponent>(OutActors[0]->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
			
			UHomeWork22GameInstance* myGI = Cast<UHomeWork22GameInstance>(GetWorld()->GetGameInstance());
			if (myGI)
			{
				WeaponToDrop.ActorToDrop = myGI->GetActorToDropByName(CurrentWeapon->WeaponSetting.WeaponName, true);
			}

			// Добавляем имя
			WeaponToDrop.WeaponName = WeaponSlots[CurrentIndex].ItemName;
			// Тип
			WeaponToDrop.WeaponType = WeaponSlots[CurrentIndex].WeaponType;
			// Патроны
			WeaponToDrop.Count = WeaponSlots[CurrentIndex].AdditionalInfo.Round;

			WeaponToDrop.MaxRound = WeaponSlots[CurrentIndex].AdditionalInfo.MaxRound;

			if (CurrentWeapon->GetOwner()->GetClass()->ImplementsInterface(UIGameActor::StaticClass()))
			{
				IIGameActor::Execute_DropWeaponItem(GetOwner(), WeaponToDrop);
			}
			WeaponSlots.RemoveAt(CurrentIndex);
			//CurrentIndex = 0;

			CurrentIndex = 1;
		}
		else
		{
			return false;
		}
	}

	return true;
}


EWeaponType UWeaponSwitchActor::ReturnWeaponType()
{
	
	return WeaponSlots[WeaponIndexToDrop].WeaponType;
}

FName UWeaponSwitchActor::GetWeaponNameByIndex(int32 IndexSlot)
{
	FName NameToReturn;
	if (WeaponSlots.IsValidIndex(IndexSlot))
	{
		NameToReturn = WeaponSlots[IndexSlot].ItemName;
	}
	
	return NameToReturn;
}

int32 UWeaponSwitchActor::GetIndexToChange()
{
	for (int i =0;i< WeaponSlots.Num();i++)
	{
		if (i != WeaponIndexToDrop)
		{
			

			return i;
		}
	}
	return 0;
}

void UWeaponSwitchActor::InitInventory_OnServer_Implementation(const TArray<FWeaponSlot>& WeaponSlotsToInit, const TArray<FAmmoSlot>& AmmoSlotToInit)
{
	WeaponSlots = WeaponSlotsToInit;
	AmmoSlots = AmmoSlotToInit;
	//BroadCast?
}


void UWeaponSwitchActor::OnSwitchWeapon_Server_Implementation(FWeaponSlot WeaponSlot)
{
	OnSwitchWeapon_Multicast(WeaponSlot);
}


void UWeaponSwitchActor::OnSwitchWeapon_Multicast_Implementation(FWeaponSlot Multicast_WeaponSlot)
{
	OnSwitchWeapon.Broadcast(Multicast_WeaponSlot);
}

//добавление оружия к инвентарю
bool UWeaponSwitchActor::TryGetWeaponToInventory(FWeaponSlot NewWeapon)
{
	int32 FreeIndexSlot = -1;
	if (CheckCanTakeWeapon(FreeIndexSlot, NewWeapon.ItemName))
	{	
		if (WeaponSlots.IsValidIndex(FreeIndexSlot))
		{
			WeaponSlots[FreeIndexSlot] = NewWeapon;
			OnInventoryWeaponPickUpSuccess_Multicast(FreeIndexSlot, WeaponSlots[FreeIndexSlot]);				
			return true;
		}
	
	}
	return false;
}

void UWeaponSwitchActor::OnInventoryWeaponPickUpSuccess_Multicast_Implementation(int32 NetFreeIndexSlot, FWeaponSlot NetWeaponSlot)
{
	OnInventoryWeaponPickUpSuccess.Broadcast(NetFreeIndexSlot, NetWeaponSlot);
}


bool UWeaponSwitchActor::CheckCanTakeWeapon(int32& FreeSlotIndex, FName _NewWeaponName)
{

	UHomeWork22GameInstance* myGI = Cast<UHomeWork22GameInstance>(GetWorld()->GetGameInstance());
	bool TempBool = false;
	FWeaponInfo TempWeaponInfo;
	if (myGI)
	{
		for (int i = 0; i < WeaponSlots.Num(); i++)
		{

			//Проверка на пустой слот БЕЗ ИМЕНИ или с ИМЕНЕМ оружия, которого НЕТ в таблице

			if (WeaponSlots[i].ItemName.IsNone()
				//Если в WeaponSlot[i] записана билиберда
				|| myGI->GetWeaponInfoByName(WeaponSlots[i].ItemName, TempWeaponInfo, TempBool) == false
				//Поиск нового оружия в таблице
				|| myGI->GetWeaponInfoByName(_NewWeaponName, TempWeaponInfo, TempBool) == false
				&& WeaponSlots.Num()< MaxWeaponInInventory)
			{
				FreeSlotIndex = i;

				//Debug
				/*if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, TEXT("FreeSlotIndex = ") + FString::FromInt(FreeSlotIndex));
				}*/
				return true;
			}
			else if (WeaponSlots.Num() < MaxWeaponInInventory)
			{
				FWeaponSlot NewWeaponSlot;
				WeaponSlots.Add(NewWeaponSlot);
				FreeSlotIndex = WeaponSlots.Num()-1;

				//Debug
				/*if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, TEXT("FreeSlotIndex = ") + FString::FromInt(FreeSlotIndex));
				}*/

				return true;
			}
		}
	}


	return false;
}



int32 UWeaponSwitchActor::GetAmmoIndexByType(EWeaponType _WeaponType)
{
	for (int i = 0; i < AmmoSlots.Num(); i++)
	{
		if (AmmoSlots[i].WeaponType == _WeaponType)
		{
			return i;
		}
	}
	return -1;
}



void UWeaponSwitchActor::AmmoSlotChangeValue(EWeaponType _WeaponType, int32 CountChangeAmmo)
{
	for (int i = 0; i<AmmoSlots.Num();i++)
	{
		if (AmmoSlots[i].WeaponType == _WeaponType)
		{
			AmmoSlots[i].Count += CountChangeAmmo;

			if (AmmoSlots[i].Count>AmmoSlots[i].MaxCount)
			{
				AmmoSlots[i].Count = AmmoSlots[i].MaxCount;
			
			}
			//broadcast for widget
			OnAmmoChange_Multicast(AmmoSlots[i].WeaponType, AmmoSlots[i].Count);
			//OnAmmoChange.Broadcast(AmmoSlots[i].WeaponType, AmmoSlots[i].Count);
		}
		
	}

	
}

void UWeaponSwitchActor::OnAmmoChange_Multicast_Implementation(EWeaponType TypeAmmo, int32 _Count)
{
	OnAmmoChange.Broadcast(TypeAmmo, _Count);
}


void UWeaponSwitchActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponSwitchActor, WeaponSlots);
	DOREPLIFETIME(UWeaponSwitchActor, AmmoSlots);
	DOREPLIFETIME(UWeaponSwitchActor, CurrentIndex);
}