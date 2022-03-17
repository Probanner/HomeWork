// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "FunctionLibrary/StateEffect.h"
#include "Engine/DataTable.h"
#include "Types.generated.h"


UENUM(BlueprintType)
 enum class EMovementState : uint8
{
	Aim_State UMETA(DisplayName="Aim State"),
	Walk_State UMETA(DisplayName = "Walk State"),
	Run_State UMETA(DisplayName ="Run State"),
	Sprint_State UMETA(DisplayName = "Sprint State")
};


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	RifleType UMETA(DisplayName = "Rifle"),
	PistolType UMETA(DisplayName = "Pistol"),
	ShotgunType UMETA(DisplayName = "Shotgun"),
	GrenadeType UMETA(DisplayName = "GrenateLauncher")
};

UENUM(BlueprintType)
enum class ERariness : uint8
{
	Common_Type UMETA(DisplayName = "Common"),
	Rare_Type UMETA(DisplayName = "Rare"),
	Epic_Type UMETA(DisplayName = "Epic"),
	Legendary_Type UMETA(DisplayName = "Legendary")
};


USTRUCT(BlueprintType)
struct FProjectileInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		TSubclassOf<class ADefaultProjectile> Projectile = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		float ProjectileDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		float ProjectileLifeTime = 8.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		float ProjectileSpeed = 2000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		float DamageDropCoef = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		float DamageDropDistance = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		float MaximumDamageDistance = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		bool BDrawDebugSphere = false;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		bool bIsLikeBomb = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		float BombRadius = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
		TSubclassOf<UStateEffect> Effect = nullptr;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		TMap<TEnumAsByte<EPhysicalSurface>, UMaterialInterface*> HitDecals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		TMap<USoundBase*, TEnumAsByte<EPhysicalSurface>> SoundsOfHits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		TMap<TEnumAsByte<EPhysicalSurface>, UParticleSystem*> HitFXs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		float ProjectileMaxRadiusDamage = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	UMaterialInterface* SkeletalBrush = nullptr;

};


USTRUCT(BlueprintType)
struct FWeaponDispersion
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
	float Aim_StateDispersionAimMax = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
	float Aim_StateDispersionAimMin = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
	float Aim_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
	float Aim_StateDispersionAimReduction = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float AimWalk_StateDispersionAimMax = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float AimWalk_StateDispersionAimMin = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float AimWalk_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float AimWalk_StateDispersionAimReduction = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float Walk_StateDispersionAimMax = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float Walk_StateDispersionAimMin = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float Walk_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float Walk_StateDispersionAimReduction = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float Run_StateDispersionAimMax = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float Run_StateDispersionAimMin = 4.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float Run_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float Run_StateDispersionAimReduction = 0.1f;


};


USTRUCT(BlueprintType)
struct FCharacterSpeed 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AimSpeed = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunSpeed = 500.0f; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 800.0f;

};

USTRUCT(BlueprintType)
struct FWeaponInfo: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Class")
	TSubclassOf<class ADefaultWeapon> WeaponClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float RateOfFire = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float ReloadTime = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	int32 MaxRound = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		int32 BulletPerShot = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		bool IsAPistol = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Name")
		FName WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponType")
	EWeaponType WeaponType = EWeaponType::RifleType;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool IsAGranadeLauncher = false;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
	FWeaponDispersion Dispersion;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* SoundFireWeapon =nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* SoundReload = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	UParticleSystem* EffectFireWeapon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	FProjectileInfo ProjectileSetting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float WeaponDamage = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float TraceDistance = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitEffect")
	UDecalComponent* DecalOnHit = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* HipAnimCharFire = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* AimAnimCharFire = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* WeaponEquipAnimMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float AnimEquipTime = 1;

	//Анимация стрельбы
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* FireGunAnim = nullptr;

	//Анимация перезарядки
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* ReloadGunAnim = nullptr;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* AnimCharReload = nullptr;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMesh* MagazineDrop = nullptr;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MagazineActor")
		TSubclassOf<class AMagazineActor> MagazineActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SleeveActor")
		TSubclassOf<class ASleeveActor> SleeveActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SleeveActor")
	float SleeveImpulseValue = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InitSocket")
		FName InitSocketName;

	//Inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UTexture2D* WeaponIcon = nullptr;

};



USTRUCT(BlueprintType)
struct FAdditionalWeaponInfo
{
	GENERATED_BODY()

	//Количество патронов в обойме
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStats")
	int32 Round = 10;
};


// Индекс, имя, и количество патронов в магазине у конкретного Weapon
USTRUCT(BlueprintType)
struct FWeaponSlot 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSlot")
		int32 IndexSlot = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSlot")
		FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponType")
		EWeaponType WeaponType = EWeaponType::RifleType;

	//Структура, там пока только количество патронов в обойме
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSlot")
		FAdditionalWeaponInfo AdditionalInfo;
	
	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoSlot")
		int32 Count = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoSlot")
		int MaxCount = 100;
	*/
		

};



USTRUCT(BlueprintType)
struct FAmmoSlot 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoSlot")
		EWeaponType WeaponType = EWeaponType::RifleType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoSlot")
		int32 Count = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoSlot")
		int32 MaxCount = 100;
};

USTRUCT(BlueprintType)
struct FDropItem : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Drop")
		TSubclassOf<class APickupActor> ActorToDrop = nullptr;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Drop")
		bool bIsAWeapon = false;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Drop")
		FName WeaponName;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Drop")
		EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Drop")
		int32 Count = 100;

	//Редкость выпадаемого предмета
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Rariness")
	ERariness LootRariness;

	
};

USTRUCT(BlueprintType)
struct FLootCase : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Case Skelet")
		USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Case Static Mesh")
		UStaticMesh* StaticMesh;

	//Редкость ящика
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case Rariness")
		ERariness LootRariness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case Rariness")
		FLinearColor LootColor;
};





UCLASS()
class HOMEWORK22_API UTypes : public UBlueprintFunctionLibrary
{

	GENERATED_BODY()

	public:
	UFUNCTION(BlueprintCallable)
	static void AddEffectBySurfaceType(AActor* HittedActor, FName ActorBoneName, TSubclassOf<UStateEffect> AddEffectClass, EPhysicalSurface SurfaceType);

};