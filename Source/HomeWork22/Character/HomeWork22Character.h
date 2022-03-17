// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/CapsuleComponent.h"
#include "CoreMinimal.h"
#include "DefaultWeapon.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "FunctionLibrary/Types.h"

#include "Net/UnrealNetwork.h"

#include "FunctionLibrary/IGameActor.h"
#include "WeaponSwitchActor.h"
#include "CharHealthComponent.h"
#include "UHomeWork22GameInstance.h"
#include "HomeWork22Character.generated.h"


UCLASS(Blueprintable)
class AHomeWork22Character : public ACharacter, public IIGameActor
{
	GENERATED_BODY()

public:
	AHomeWork22Character();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UStaticMeshComponent* GetCursorToWorld() { return CursorToWorld; }

	//Присваиваем указателю капсуль актора
	UCapsuleComponent* MyCapsule = this->GetCapsuleComponent();

	FTimerHandle RagDollTimer;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* CursorToWorld;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UWeaponSwitchActor* WeaponSwitchComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
		class UCharHealthComponent* CharHealthComp;




protected:
	virtual void BeginPlay() override;

public:

	DECLARE_DELEGATE_OneParam(DELEGATE_ShiftPressed, bool);
	DECLARE_DELEGATE_OneParam(DELEGATE_AltPressed, bool);
	DECLARE_DELEGATE_OneParam(DELEGATE_RMBPressed, bool);
	DECLARE_DELEGATE_OneParam(DELEGATE_OnenumberPressed, int32);
	DECLARE_DELEGATE_OneParam(DELEGATE_TwonumberPressed, int32);
	DECLARE_DELEGATE_OneParam(DELEGATE_ThreenumberPressed, int32);
	DECLARE_DELEGATE_OneParam(DELEGATE_FournumberPressed, int32);

	UPROPERTY(Replicated, BlueprintReadOnly)
		EMovementState MovementState = EMovementState::Sprint_State;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FCharacterSpeed MovementInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool ShiftPressed = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool AltPressed = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", Replicated)
		bool RMBPressed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DeathAnim")
		TArray<UAnimMontage*> DeathAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool RMBWasPressed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
		bool InputBlock = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReloadSystem",Replicated)
		bool IsReloading = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityEffect")
		TSubclassOf<UStateEffect> AbilityEffect;


	//Текущее оружие
	UPROPERTY(Replicated)
	ADefaultWeapon* CurrentWeapon = nullptr;
	FVector CursorHitResultLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		FName InitWeaponName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pistol",Replicated)
		bool PistolEquiped = false;

	//for demo

	UFUNCTION()
		void SprintFunction(bool BSprint_local);
	UFUNCTION()
		void WalkFunction(bool BWalk_local);
	UFUNCTION()
		void AimFunction(bool BAim_local);


	//Приводим вектор скорости к нормали
	FVector NormVelocity;

	UFUNCTION()
		void InputAxisX(float Value);
	UFUNCTION()
		void InputAxisY(float Value);

	float AxisX = 0.0f;
	float AxisY = 0.0f;

	//Movement tick
	UFUNCTION()
		void MovementTick(float DeltaTime);

	UFUNCTION(BlueprintCallable)
		void CharacterUpdate();

	UFUNCTION(BlueprintNativeEvent)
		void WeaponReloadStart_BP();

	UFUNCTION(BlueprintNativeEvent)
		void WeaponReloadEnd_BP();


	UFUNCTION(BlueprintCallable)
		void ChangeMovementState();
	UFUNCTION(BlueprintCallable)
		ADefaultWeapon* GetCurrentWeapon();

	UFUNCTION(BlueprintCallable)
		void WeaponInit(FWeaponSlot _NewWeaponSlot);

	UFUNCTION()
		void InputAttackPressed();
	UFUNCTION()
		void InputAttackReleased();

	void AttackCharEvent(bool bIsFiring);
	void TryReloadWeapon();

	//for delegates
	UFUNCTION()
		void WeaponReloadStart();

	UFUNCTION()
		void WeaponReloadEnd();

	UFUNCTION()
		void FireMontage(bool _bHaveAnim);

	void WeaponChangeCharacterForDelegate(int32 ChangeToIndex);

	UFUNCTION(BlueprintCallable)
		bool WeaponChangeCharacter(int32 ChangeToIndex);


	void StopAllAminChar();

	UFUNCTION(BlueprintCallable)
		int GetAmmoTypeIndex();


	UFUNCTION(BlueprintCallable)
		UHomeWork22GameInstance* GetCharacterGameInstance();

	UFUNCTION()
		void CharOnDeath();

	void RagDollEnable();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool bIsAlive = true;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;


	//Interface

	//Effect
	TArray<UStateEffect*> Effects;

	TArray<UStateEffect*> GetAllCurrentEffects() override;
	void RemoveEffect(UStateEffect* RemoveEffect) override;
	void AddEffect(UStateEffect* NewEffect) override;

	UFUNCTION(BlueprintCallable)
		EPhysicalSurface GetSurfaceType() override;

	//Ability Function
	UFUNCTION()
		void TryAbilityEnabled();

	//EndOfInterface

	UFUNCTION(BlueprintNativeEvent)
		void SpawnDamageWidget(float CurrentHealth, float Damage);

	void DropCurrentWeaponChar();

	UPROPERTY(Replicated)
	UAnimMontage* ReloadCharAnim;

	UFUNCTION(BlueprintNativeEvent)
		void CharDead_BP();

	UFUNCTION(Server, Unreliable)
		void SetActorRotationByYaw_OnServer(float _Yaw);

	UFUNCTION(NetMulticast, Unreliable)
		void SetActorRotationByYaw_MultiCast(float _Yaw);

	UFUNCTION(Server, Reliable)
		void SetMovementState_OnServer(EMovementState NewState);

	UFUNCTION(NetMulticast, Reliable)
		void SetMovementState_Multicast(EMovementState NewState);

	UFUNCTION(NetMulticast, Reliable)
	void WeaponEquipAnimChar_Anim_Multicast(UAnimMontage* AnimChangeMontage, float PlayRate);

	UFUNCTION(NetMulticast, Reliable)
	void ChangeCharAimState_Multicast(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ChangeCharAimState_Server(bool bIsAiming);

	UFUNCTION(NetMulticast, Reliable)
	void StartReloadChar_Multicast(UAnimMontage* ReloadMontage, float ReloadTime);


	UFUNCTION(Server, Reliable)
	void ReloadInit_Multicast();

	UFUNCTION(Server, Reliable)
	void StopAllAnim_Server();

};

