// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"


#include "Components/StaticMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "DefaultProjectile.h" 
#include "SleeveActor.h"
#include "FunctionLibrary/Types.h"
#include "DefaultWeapon.generated.h"


//class UWeaponSwitchActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponReloadStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponReloadEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFire,bool,bHaveAnim);


UCLASS()
class HOMEWORK22_API ADefaultWeapon : public AActor
{

	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	ADefaultWeapon();
	//Здесь, в родительском классе мы создаем:

	// 1) Создаем указатель - переменную на компонент сцены, которая содержит в себе Location - логику
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USceneComponent* SceneComponent = nullptr;
	// 2) Создаем указатель-переменную на мэш со скилетом; 
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USkeletalMeshComponent* SkeletalMeshWeapon = nullptr;
	// 3) Создаем указатель - переменную на обычный статик мэш;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UStaticMeshComponent* StaticMeshWeapon = nullptr;
	// 4) Создаем указатель - переменную на стрелку.
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UArrowComponent* ShootLocationArrow = nullptr;

	

	UPROPERTY(VisibleAnyWhere)
		FWeaponInfo WeaponSetting;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
		FAdditionalWeaponInfo AdditionalWeaponInfo;


	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FOnWeaponReloadStart OnWeaponReloadStart;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FOnWeaponReloadEnd OnWeaponReloadEnd;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FOnFire OnFire;

	FName CurrentWeaponIdName;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Функции для Tick'а
	void FireTick(float DeltaTime);
	void ReloadTick(float DeltaTime);
	void DispertionTick(float DeltaTime);
	void WeaponInit();

	UPROPERTY(EditAnywhere, BlueprintreadWrite, Category = "FireLogic")
		bool WeaponFiring = false;
	UPROPERTY(EditAnywhere, BlueprintreadWrite, Category = "FireLogic", Replicated)
		bool WeaponReloading = false;
	


	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetWeaponStateFire_OnServer(bool blsFire);

	UFUNCTION(BlueprintCallable)
	FWeaponInfo GetWeaponSetting();

	bool CheckWeaponCanFire();

	FProjectileInfo GetProjectile();
	void Fire();

	UFUNCTION(Server,Reliable)
	void UpdateStateWeapon_OnServer(EMovementState NewMovementState);

	void ChangeDispertion();
	float FireTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintreadWrite, Category = "ReloadLogic")
	float ReloadTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintreadWrite, Category = "ReloadLogicDebug")
	float DebugReloadTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintreadWrite, Category = "DispertionLogicDebug")
		bool byBarrel = false;

	UPROPERTY(EditAnywhere, BlueprintreadWrite, Category = "DispertionLogicDebug")
		bool ShowDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Equiped")
		bool bIsEquiped = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Skeletal Mesh")
		USkeletalMesh* SkeletalMeshVar = nullptr;

	bool BlockFire = false;

	int32 MaxRound = 40;

	FVector ShootEndLocation = FVector(0);

	/*UFUNCTION(BlueprintImplementableEvent)
		void ProjectileRespawn(TSubclassOf<ADefaultProjectile> _ProjectileToRespawn,
			float _initialSpeed, FVector _SpawnLocation,
			FRotator _SpawnRotation,
			float _SpanLifeTime,
			FProjectileInfo _ProjectileInfo);*/

	UFUNCTION(BlueprintCallable)
	int32 GetWeaponRound();
	void InitReload();
	void FinishReload();

	FVector GetFireEndLocation() const;
	FVector ApplyDispertionToShoot(FVector DispertionShoot) const;
	float GetCurrentDispertion() const;

	//Dispertion
	bool ShouldReduceDispertion = false;
	float CurrentDispertion = 0.0f;
	float CurrentDispertionMax = 1.0f;
	float CurrentDispertionMin = 1.0f;
	float CurrentDispertionRecoil = 0.1f;
	float CurrentDispertionReduction = 0.1f;

	void ChangeDispertionByShot();


	UAudioComponent* ReloadSound;

	int InventoryWeaponCount = 0;

	int32 GetBulletPerShot();

	bool CanWeaponReload();

	UFUNCTION(BlueprintCallable)
	int32 GetAvailableAmmo();

	UFUNCTION(Server,Unreliable)
	void UpdateWeaponByCharacterMovement_OnServer(FVector NewShootEndLocation, bool NewShouldReduceDispertion);

	UFUNCTION(NetMulticast, Unreliable)
	void SpawnMuzzleEmitter_Multicast(UParticleSystem* MuzzleFire, FTransform MuzzleLocation);

	UFUNCTION(NetMulticast, Unreliable)
	void SpawnMuzzleSound_Multicast(USoundBase* MuzzleSound, FVector MuzzleLocation);

	UFUNCTION(NetMulticast, Unreliable)
	void AnimWeapon_Multicast(UAnimationAsset* FireGunAnim);

	UFUNCTION(Client, Unreliable)
	void PlayReloadSoundWeapon_Client(USoundBase* ReloadSoundWeapon);

};
