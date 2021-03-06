// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunctionLibrary/Types.h"
#include "Engine/Engine.h"
#include "UHomeWork22GameInstance.h"
#include "Components/ActorComponent.h"
#include "WeaponSwitchActor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchWeapon, FWeaponSlot, WeaponSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChange, EWeaponType, TypeAmmo, int32, _Count);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoEmpty, EWeaponType, DWeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFireInventory, int32, CurrentWeaponIndexInventory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadInInventory, int32, CurrentWeaponIndexInventory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryWeaponPickUpSuccess, int32, IndexOfNewWeapon , FWeaponSlot, WeaponSlot);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOMEWORK22_API UWeaponSwitchActor : public UActorComponent
{
	GENERATED_BODY()


public:	
	// Sets default values for this component's properties
	UWeaponSwitchActor();

	//??????? ?? ????? ??????
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FOnSwitchWeapon OnSwitchWeapon;

	UPROPERTY(BlueprintAssignable,EditAnywhere,BlueprintReadWrite, Category = "Inventory")
	FOnAmmoChange OnAmmoChange;

	//???????, ????? ??????????? ???????
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FOnWeaponAmmoEmpty OnWeaponAmmoEmpty;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FOnFireInventory OnFireInventory;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FOnReloadInInventory OnReloadInInventory;

	//??????? ????? ????????? ?????????? ?????? ? ?????????
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FOnInventoryWeaponPickUpSuccess OnInventoryWeaponPickUpSuccess;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapons", Replicated)
	TArray<FWeaponSlot> WeaponSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapons", Replicated)
	TArray<FAmmoSlot> AmmoSlots;

	UPROPERTY(BlueprintReadOnly)
	int32 WeaponIndexToDrop = 0;

	UPROPERTY(BlueprintReadOnly, Replicated)
		int32 CurrentIndex = 0;


	UFUNCTION(BlueprintCallable)
	void SwitchWeaponToIndex(int32 ChangeToIndex);


	UFUNCTION(BlueprintCallable)
	int32 GetWeaponIndexSlotByName(FName IdWeaponName);

	void SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo);

	bool CharEquipedPistol = false;

	bool CheckAmmoForWeapon(EWeaponType _TypeWeapon);
	void FOnFireInInventory();
	void ReloadInInventory();

	UFUNCTION(BlueprintCallable)
	int32 ReturnLastingAmmo(EWeaponType _TypeWeapon);

	//??????? ???????? ???????? ??? ?? ???????
	UFUNCTION(BlueprintCallable)
	void AmmoSlotChangeValue(EWeaponType _WeaponType, int32 CountChangeAmmo);

	int32 MaxWeaponInInventory = 3;

	//Interface Pickup Actors
	UFUNCTION(BlueprintCallable, Category = "Item pickup Interface")
	bool CheckCanTakeAmmo(EWeaponType AmmoType);
	UFUNCTION(BlueprintCallable, Category = "Item pickup Interface")
	bool CheckCanTakeWeapon(int32 &FreeSlotIndex, FName _NewWeaponName);
	UFUNCTION(BlueprintCallable, Category = "Item pickup Interface")
	void SaveItemToInventory();

	

	UFUNCTION(BlueprintCallable, Category = "Item pickup Interface")
	void SwitchWeaponToInventory(FWeaponSlot NewWeapon, int32 IndexOfSlot);

	//?????????? ?????? ? ?????????
	UFUNCTION(BlueprintCallable, Category = "Item pickup Interface")
	bool TryGetWeaponToInventory(FWeaponSlot NewWeapon);

	int32 GetAmmoIndexByType(EWeaponType _WeaponType);

	UFUNCTION(BlueprintCallable, Category = "Item pickup Interface")
	bool DropCurrentWeaponFromInventory(ADefaultWeapon* CurrentWeapon);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	EWeaponType ReturnWeaponType();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FName GetWeaponNameByIndex(int32 IndexSlot);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetIndexToChange();


	/*UFUNCTION(BlueprintCallable, Category = "Inventory")
		int32 GetCurrentIndex();*/

	UFUNCTION(Server,BlueprintCallable, Category = "Inventory",Reliable)
	void InitInventory_OnServer(const TArray<FWeaponSlot>& WeaponSlotsToInit, const TArray<FAmmoSlot>& AmmoSlotToInit);


	//Network

	//Widgets delegates
	UFUNCTION(Server, Reliable, Category = "Inventory")
	void OnSwitchWeapon_Server(FWeaponSlot WeaponSlot);

	UFUNCTION(NetMulticast, Reliable, Category = "Inventory")
	void OnSwitchWeapon_Multicast(FWeaponSlot Multicast_WeaponSlot);

	UFUNCTION(NetMulticast, Reliable, Category = "Inventory")
	void OnAmmoChange_Multicast(EWeaponType TypeAmmo, int32 _Count);

	UFUNCTION(NetMulticast, Reliable, Category = "Inventory")
	void OnWeaponAmmoEmpty_Multicast(EWeaponType DWeaponType);

	UFUNCTION(NetMulticast, Reliable, Category = "Inventory")
	void OnFireInventory_Multicast(int32 CurrentWeaponIndexInventory);

	UFUNCTION(NetMulticast, Reliable, Category = "Inventory")
	void OnReloadInInventory_Multicast(int32 CurrentWeaponIndexInventory);

	UFUNCTION(Client, Reliable, Category = "Inventory")
	void OnInventoryWeaponPickUpSuccess_Multicast(int32 NetFreeIndexSlot, FWeaponSlot NetWeaponSlot);

};
