// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "HomeWork22Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "HomeWork22.h"


#include "Kismet/KismetMathLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Engine.h"

AHomeWork22Character::AHomeWork22Character()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//Switch Weapon 
	WeaponSwitchComponent = CreateDefaultSubobject<UWeaponSwitchActor>(TEXT("WeaponSwitch"));

	if (WeaponSwitchComponent)
	{
		WeaponSwitchComponent->OnSwitchWeapon.AddDynamic(this, &AHomeWork22Character::WeaponInit);
	}

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UStaticMeshComponent>("CursorToWorld");
	CharHealthComp = CreateDefaultSubobject<UCharHealthComponent>("CharHealthComp");

	if (CharHealthComp)
	{
		CharHealthComp->OnDead.AddDynamic(this, &AHomeWork22Character::CharOnDeath);
		CharHealthComp->OnShieldChange.AddDynamic(this, &AHomeWork22Character::SpawnDamageWidget);
		CharHealthComp->OnHeathChange.AddDynamic(this, &AHomeWork22Character::SpawnDamageWidget);
	}

	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/Blueprint/Characters/M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetMaterial(0, DecalMaterialAsset.Object);
	}
	CursorToWorld->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f).Quaternion());
	CursorToWorld->SetVisibility(false);


	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//NetWork
	
	this->SetReplicates(true);
	

	//Радиус репликации.
	//NetCullDistanceSquared = 99999;
	//Частота репликации (раз в секунду). 
	//NetUpdateFrequency = 1.f;

}

void AHomeWork22Character::Tick(float DeltaSeconds)
{

	Super::Tick(DeltaSeconds);
	if (!InputBlock)
	{
		MovementTick(DeltaSeconds);
	}

	if (CursorToWorld != nullptr)
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			if (UWorld* World = GetWorld())
			{
				FHitResult HitResult;
				FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
				FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
				FVector EndLocation = TopDownCameraComponent->GetComponentRotation().Vector() * 2000.0f;
				Params.AddIgnoredActor(this);
				World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_GameTraceChannel1, Params);
				FQuat SurfaceRotation = HitResult.ImpactNormal.ToOrientationRotator().Quaternion();
				CursorToWorld->SetWorldLocationAndRotation(HitResult.Location, SurfaceRotation);
			}
		}
		else if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{

			FHitResult TraceHitResult; // Это структура удара или информация об ударе
			PC->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery4, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = UKismetMathLibrary::MakeRotFromZ(CursorFV);
			CursorToWorld->SetWorldLocation(TraceHitResult.Location); 
			CursorToWorld->SetWorldRotation(CursorR); 

		}
	}
}

//ИМПУТЫ
void AHomeWork22Character::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
	Super::SetupPlayerInputComponent(NewInputComponent);

	NewInputComponent->BindAxis(TEXT("MoveForward"), this, &AHomeWork22Character::InputAxisX);
	NewInputComponent->BindAxis(TEXT("MoveRight"), this, &AHomeWork22Character::InputAxisY);

	//ЛКМ
	NewInputComponent->BindAction(TEXT("LeftMouseButton"), EInputEvent::IE_Pressed, this, &AHomeWork22Character::InputAttackPressed);
	NewInputComponent->BindAction(TEXT("LeftMouseButton"), EInputEvent::IE_Released, this, &AHomeWork22Character::InputAttackReleased);

	//Перезарядка
	NewInputComponent->BindAction(TEXT("ReloadEvent"), EInputEvent::IE_Released, this, &AHomeWork22Character::TryReloadWeapon);


	//Нажимаем левый шифт
	NewInputComponent->BindAction<DELEGATE_ShiftPressed>("LeftShiftPress", IE_Pressed, this, &AHomeWork22Character::SprintFunction, true);
	//Отпускаем левый шифт
	NewInputComponent->BindAction<DELEGATE_ShiftPressed>("LeftShiftPress", IE_Released, this, &AHomeWork22Character::SprintFunction, false);
	//Нажимаем левый альт
	NewInputComponent->BindAction<DELEGATE_AltPressed>("AltWalk", IE_Pressed, this, &AHomeWork22Character::WalkFunction, true);
	//Отпускаем левый альт
	NewInputComponent->BindAction<DELEGATE_AltPressed>("AltWalk", IE_Released, this, &AHomeWork22Character::WalkFunction, false);
	// ПКМ нажимаем
	NewInputComponent->BindAction<DELEGATE_RMBPressed>("RightMouseAim", IE_Pressed, this, &AHomeWork22Character::AimFunction, true);
	// ПКМ отпускаем
	NewInputComponent->BindAction<DELEGATE_RMBPressed>("RightMouseAim", IE_Released, this, &AHomeWork22Character::AimFunction, false);

	NewInputComponent->BindAction<DELEGATE_OnenumberPressed>("SwitchWeapon0", IE_Pressed, this, &AHomeWork22Character::WeaponChangeCharacterForDelegate_OnServer, int32(0));

	NewInputComponent->BindAction<DELEGATE_TwonumberPressed>("SwitchWeapon1", IE_Pressed, this, &AHomeWork22Character::WeaponChangeCharacterForDelegate_OnServer, int32(1));

	NewInputComponent->BindAction<DELEGATE_ThreenumberPressed>("SwitchWeapon2", IE_Pressed, this, &AHomeWork22Character::WeaponChangeCharacterForDelegate_OnServer, int32(2));

	NewInputComponent->BindAction<DELEGATE_FournumberPressed>("SwitchWeapon3", IE_Pressed, this, &AHomeWork22Character::WeaponChangeCharacterForDelegate_OnServer, int32(3));


	//NewInputComponent->BindAction("AbilityEnable", IE_Pressed, this, &AHomeWork22Character::TryAbilityEnabled);

	NewInputComponent->BindAction("DropCurrentWeapon", IE_Pressed, this, &AHomeWork22Character::DropCurrentWeaponChar);

}

void AHomeWork22Character::SprintFunction(bool BSprint_local)
{

	if (BSprint_local)
	{
		ShiftPressed = true;
	}
	if (!BSprint_local)
	{
		ShiftPressed = false;
	}
	ChangeMovementState();
}

void AHomeWork22Character::WalkFunction(bool BWalk_local)
{
	if (BWalk_local)
	{
		AltPressed = true;
	}
	else
	{
		AltPressed = false;
	}
	ChangeMovementState();
}

void AHomeWork22Character::AimFunction(bool BAim_local)
{
	ChangeCharAimState_Server(BAim_local);
}

void AHomeWork22Character::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld() && GetWorld()->GetNetMode() != NM_DedicatedServer)
	{
		if (CursorToWorld && GetLocalRole() == ROLE_AutonomousProxy || GetLocalRole() == ROLE_Authority)
		{
			CursorToWorld->SetVisibility(true);
		}
	}
}

void AHomeWork22Character::InputAxisY(float Value)
{
	AxisY = Value;
}

void AHomeWork22Character::InputAxisX(float Value)
{
	AxisX = Value;
}

void AHomeWork22Character::MovementTick(float DeltaTime)
{
	if (bIsAlive)
	{

		if (GetController() && GetController()->IsLocalPlayerController())
		{
			AddMovementInput(FVector(1.0f, 0.0f, 0.0f), AxisX);
			AddMovementInput(FVector(0.0f, 1.0f, 0.0f), AxisY);

			APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

			if (myController && myController->IsLocalPlayerController())
			{

				FHitResult HitResult;

				myController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery4, true, HitResult);
				CursorHitResultLocation = HitResult.Location;
				//поворот персонажа на курсор
				FRotator myRotator = FRotator(0.0f, UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), HitResult.Location).Yaw, 0.0f);

				SetActorRotation(FQuat(myRotator));

				SetActorRotationByYaw_OnServer(myRotator.Yaw);

				if (CurrentWeapon)
				{
					FVector Displacement = FVector(0);
					bool bIsReduceDispersion = false;
					switch (MovementState)
					{
					case EMovementState::Aim_State:
						Displacement = FVector(0.0f, 0.0f, 160.0f);
						bIsReduceDispersion = true;
						break;
					case EMovementState::Walk_State:
						Displacement = FVector(0.0f, 0.0f, 140.0f);
						bIsReduceDispersion = true;
						break;
					case EMovementState::Run_State:
						Displacement = FVector(0.0f, 0.0f, 120.0f);
						break;
					case EMovementState::Sprint_State:
						Displacement = FVector(0.0f, 0.0f, 160.0f);
						break;
					default:
						break;
					}

					if (CurrentWeapon)
					{
						CurrentWeapon->UpdateWeaponByCharacterMovement_OnServer(HitResult.Location, bIsReduceDispersion);
					}
				}
			}
		}
	}
}

void AHomeWork22Character::CharacterUpdate()
{

	float ResSpeed = 500.0f;
	switch (MovementState)
	{
	case EMovementState::Aim_State:
		ResSpeed = MovementInfo.AimSpeed;
		break;
	case EMovementState::Walk_State:
		ResSpeed = MovementInfo.WalkSpeed;
		break;
	case EMovementState::Run_State:
		ResSpeed = MovementInfo.RunSpeed;
		break;
	case EMovementState::Sprint_State:
		ResSpeed = MovementInfo.SprintSpeed;
		break;
	default:
		break;
	}

	GetCharacterMovement()->MaxWalkSpeed = ResSpeed;

	ADefaultWeapon* myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		myWeapon->UpdateStateWeapon_OnServer(MovementState);
	}

}

void AHomeWork22Character::ChangeMovementState()
{

	EMovementState NewMovementState = EMovementState::Run_State;
	if (!InputBlock)
	{
		//MovementTick(DeltaSeconds);

		NormVelocity = this->GetVelocity();
		NormVelocity.Normalize(0.0001);
		// Movement Logic
		if (ShiftPressed && !AltPressed && !RMBPressed)
		{
			NewMovementState = EMovementState::Sprint_State;
		}
		else if (AltPressed && !ShiftPressed && !RMBPressed)
		{
			NewMovementState = EMovementState::Walk_State;
		}
		else if (RMBPressed && !AltPressed && !ShiftPressed)
		{
			NewMovementState = EMovementState::Aim_State;
		}
		else if (!ShiftPressed && !AltPressed && !ShiftPressed && !RMBPressed)
		{
			NewMovementState = EMovementState::Run_State;
		}
		else if (RMBPressed && ShiftPressed && AltPressed)
		{
			NewMovementState = EMovementState::Walk_State;
		}
		else if (RMBPressed && AltPressed)
		{
			NewMovementState = EMovementState::Walk_State;
		}
		else if (RMBPressed && ShiftPressed)
		{
			NewMovementState = EMovementState::Aim_State;
		}
		else
		{
			NewMovementState = EMovementState::Run_State;
		}
	}
	
	SetMovementState_OnServer(NewMovementState);

	//Weapon state update
	ADefaultWeapon* myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		myWeapon->UpdateStateWeapon_OnServer(NewMovementState);
	}
}

ADefaultWeapon* AHomeWork22Character::GetCurrentWeapon()
{
	return CurrentWeapon;
}


//Инициализация оружия
void AHomeWork22Character::WeaponInit(FWeaponSlot _NewWeaponSlot)
{
 //OnServer
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	UHomeWork22GameInstance* myGI = Cast<UHomeWork22GameInstance>(GetGameInstance());
	FWeaponInfo myWeaponInfo;
	if (myGI)
	{

		if (myGI->GetWeaponInfoByName(_NewWeaponSlot.ItemName, myWeaponInfo, PistolEquiped))
		{
		
			if (myWeaponInfo.WeaponClass)
			{
				//создаем спаун-локацию типа вектора с тремя координатами
				FVector SpawnLocation = FVector(0);
				// Создаем ротатора с нулевым поворотом по всем осям
				FRotator SpawnRotation = FRotator(0);

				//Структура, которая содержит в себе параметры для спауна 
				FActorSpawnParameters SpawnParams;
				//такие как:
				//Настройки коллизии при спауне актора оружия:
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				//Получаем актора, который владеет заресанным оружием, он же AHomeWork22PlayerController
				SpawnParams.Owner = this;
				//Получаем актора, который инициирует урон из заспауненного актора-оружия - в данном случае, Instigator'ом является AHomeWork22Character
				SpawnParams.Instigator = GetInstigator();
				//Спауним оружие

				if (myWeaponInfo.WeaponEquipAnimMontage)
				{
					WeaponEquipAnimChar_Anim_Multicast(myWeaponInfo.WeaponEquipAnimMontage, myWeaponInfo.WeaponEquipAnimMontage->GetMaxCurrentTime() / myWeaponInfo.AnimEquipTime);
					
				}

				ADefaultWeapon* myWeapon = Cast<ADefaultWeapon>(GetWorld()->SpawnActor(myWeaponInfo.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
				//Всегда проверяй УКАЗАТЕЛЬ!!!
				if (myWeapon)
				{
					//Создаем струтуру правил аттача для компонента
					FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
					//тут мы аттачим оружие к руке. GetMesh получает мэш скелета.

					myWeapon->AttachToComponent(GetMesh(), Rule, myWeaponInfo.InitSocketName);


					//Указателю CurrentWeapon присваиваем нового актора
					CurrentWeapon = myWeapon;
					//myWeapon->InventoryWeaponCount = WeaponSwitchComponent->AmmoSlots[GetAmmoTypeIndex()].Count;
					myWeapon->AdditionalWeaponInfo = _NewWeaponSlot.AdditionalInfo;
					myWeapon->WeaponSetting = myWeaponInfo;
					myWeapon->WeaponSetting.WeaponType = _NewWeaponSlot.WeaponType;
					myWeapon->CurrentWeaponIdName = _NewWeaponSlot.ItemName;
					myWeapon->AdditionalWeaponInfo = _NewWeaponSlot.AdditionalInfo;
					myWeapon->DebugReloadTime = myWeaponInfo.ReloadTime;
					myWeapon->OnWeaponReloadStart.AddDynamic(this, &AHomeWork22Character::WeaponReloadStart);
					myWeapon->OnWeaponReloadEnd.AddDynamic(this, &AHomeWork22Character::WeaponReloadEnd);
					myWeapon->OnFire.AddDynamic(this, &AHomeWork22Character::FireMontage);
					myWeapon->UpdateStateWeapon_OnServer(MovementState);
					myWeapon->SkeletalMeshWeapon->SetVisibility(false);


				}
			}
		}
		else
		{
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("HomeWork22Character::WeaponInit - Weapon not found"));
		}
	}
}

void AHomeWork22Character::InputAttackPressed()
{
	if (bIsAlive)
	{
		AttackCharEvent(true);
	}
}

void AHomeWork22Character::InputAttackReleased()
{
	AttackCharEvent(false);
}

void AHomeWork22Character::AttackCharEvent(bool bIsFiring)
{
	ADefaultWeapon* myWeapon = nullptr;
	myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		myWeapon->SetWeaponStateFire_OnServer(bIsFiring);
	}

}

void AHomeWork22Character::TryReloadWeapon()
{
	if (GetAmmoTypeIndex() != -1 && bIsAlive)
	{
	FWeaponSlot CurrentWeaponInInventory = WeaponSwitchComponent->WeaponSlots[WeaponSwitchComponent->CurrentIndex];
	

		if (CurrentWeapon && WeaponSwitchComponent->AmmoSlots[GetAmmoTypeIndex()].Count != 0)
		{
			if (CurrentWeaponInInventory.AdditionalInfo.Round < CurrentWeaponInInventory.AdditionalInfo.MaxRound && !CurrentWeapon->WeaponReloading)
			{
				ReloadInit_Server();			
			}
		}
	}

}

void AHomeWork22Character::WeaponReloadStart()
{
	//WeaponReloadStart_BP();
	IsReloading = true;

	if (GetCurrentWeapon()->WeaponSetting.AnimCharReload)
	{
	ReloadCharAnim = GetCurrentWeapon()->WeaponSetting.AnimCharReload;
	StartReloadChar_Multicast(ReloadCharAnim, GetCurrentWeapon()->WeaponSetting.AnimCharReload->GetMaxCurrentTime() / GetCurrentWeapon()->WeaponSetting.ReloadTime);
	}

}

void AHomeWork22Character::WeaponReloadEnd()
{
	IsReloading = false;
}

// Проверка на соответсвие патрона и текущего оружия
int AHomeWork22Character::GetAmmoTypeIndex()
{
	for (int i = 0; i < WeaponSwitchComponent->AmmoSlots.Num(); i++)
	{	
	//Проверка типа оружия через инвентарь
		if (GetCurrentWeapon() && (WeaponSwitchComponent->AmmoSlots[i].WeaponType == WeaponSwitchComponent->WeaponSlots[WeaponSwitchComponent->CurrentIndex].WeaponType))
		{
			return i;
		}
	}
	return -1;
}





UHomeWork22GameInstance* AHomeWork22Character::GetCharacterGameInstance()
{
	UHomeWork22GameInstance* myGI = Cast<UHomeWork22GameInstance>(GetWorld()->GetGameInstance());
	return myGI;
}

void AHomeWork22Character::FireMontage(bool _bHaveAnim)
{
	if (_bHaveAnim)
	{
		if (GetCurrentWeapon()->WeaponSetting.HipAnimCharFire && !RMBPressed)
		{
			WeaponEquipAnimChar_Anim_Multicast(GetCurrentWeapon()->WeaponSetting.HipAnimCharFire,1);
			//PlayAnimMontage(GetCurrentWeapon()->WeaponSetting.HipAnimCharFire, 1);
		}
		if (GetCurrentWeapon()->WeaponSetting.AimAnimCharFire && RMBPressed)
		{
			WeaponEquipAnimChar_Anim_Multicast(GetCurrentWeapon()->WeaponSetting.AimAnimCharFire, 1);
			//PlayAnimMontage(GetCurrentWeapon()->WeaponSetting.AimAnimCharFire, 1);
		}
	}

	WeaponSwitchComponent->WeaponSlots[WeaponSwitchComponent->CurrentIndex].AdditionalInfo.Round = GetCurrentWeapon()->GetWeaponRound();

}



//Сюда присылаем
void AHomeWork22Character::WeaponChangeCharacterForDelegate_OnServer_Implementation(int32 ChangeToIndex)
{
	UHomeWork22GameInstance* myGI = Cast<UHomeWork22GameInstance>(GetWorld()->GetGameInstance());

	if (myGI)
	{
		if (ChangeToIndex + 1 <= WeaponSwitchComponent->WeaponSlots.Num()
			&& ChangeToIndex != WeaponSwitchComponent->CurrentIndex
			&& !WeaponSwitchComponent->WeaponSlots[ChangeToIndex].ItemName.IsNone()
			&& myGI->GetWeaponInfoByName(WeaponSwitchComponent->WeaponSlots[ChangeToIndex].ItemName, GetCurrentWeapon()->WeaponSetting, PistolEquiped))
		{
			StopAllAminChar();
			GetCurrentWeapon()->OnWeaponReloadEnd.Broadcast();

			WeaponChangeCharacterForDelegate_Multicast(ChangeToIndex);
			//WeaponSwitchComponent->SwitchWeaponToIndex(ChangeToIndex, GetCurrentWeapon()->AdditionalWeaponInfo);
		}
		else
		{	//Debug
			//GEngine->AddOnScreenDebugMessage(-1,10,FColor::Orange, TEXT("WeaponChangeCharacterForDelegate_OnServer_Implementation - FALSE"));
		}
	}
}


void AHomeWork22Character::WeaponChangeCharacterForDelegate_Multicast_Implementation(int32 NetChangeToIndex)
{
	WeaponSwitchComponent->SwitchWeaponToIndex(NetChangeToIndex);
}


void AHomeWork22Character::StopAllAminChar()
{
	StopAllAnim_Server();
}




void AHomeWork22Character::WeaponReloadStart_BP_Implementation()
{

}

void AHomeWork22Character::WeaponReloadEnd_BP_Implementation()
{

}

void AHomeWork22Character::CharOnDeath()
{
	if (DeathAnimMontage.Num() > 0)
	{
		int32 RandomMontageIndex = FMath::RandRange(0, DeathAnimMontage.Num() - 1);
		GetMesh()->GetAnimInstance()->Montage_Play(DeathAnimMontage[RandomMontageIndex]);

		float AnimTimeDeath = DeathAnimMontage[RandomMontageIndex]->GetMaxCurrentTime() - 0.3;

		GetWorld()->GetTimerManager().SetTimer(RagDollTimer, this, &AHomeWork22Character::RagDollEnable, AnimTimeDeath, false);

	}

	GetCurrentWeapon()->WeaponFiring = false;

	//if (GetController())
	//{
	//	GetController()->UnPossess();
	//}
	//UnPossessed();
	bIsAlive = false;
	GetCursorToWorld()->SetVisibility(false);
	CharDead_BP();

}

void AHomeWork22Character::CharDead_BP_Implementation()
{
	//BP
}





void AHomeWork22Character::RagDollEnable()
{
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

		GetMesh()->SetSimulatePhysics(true);
	}
}

float AHomeWork22Character::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (bIsAlive)
	{
		UpdateCharHealth_Multicast(-ActualDamage);
		//CharHealthComp->ChangeCurrentHealth(-ActualDamage);
	}
	return ActualDamage;
}

TArray<UStateEffect*> AHomeWork22Character::GetAllCurrentEffects()
{
	return Effects;
}

void AHomeWork22Character::RemoveEffect(UStateEffect* RemoveEffect)
{
	Effects.Remove(RemoveEffect);
}

void AHomeWork22Character::AddEffect(UStateEffect* NewEffect)
{
	Effects.Add(NewEffect);
}

EPhysicalSurface AHomeWork22Character::GetSurfaceType()
{
	EPhysicalSurface Result = EPhysicalSurface::SurfaceType_Default;

	if (CharHealthComp)
	{
		if (CharHealthComp->GetCurrentShield() <= 0)
		{
			if (GetMesh())
			{
				UMaterialInterface* myMaterial = GetMesh()->GetMaterial(0);
				if (myMaterial)
				{
					Result = myMaterial->GetPhysicalMaterial()->SurfaceType;
				}
			}
		}
	}

	return Result;
}

//void AHomeWork22Character::TryAbilityEnabled()
//{
//	if (AbilityEffect)// TODO cooldown
//	{
//		UStateEffect* NewEffect = NewObject<UStateEffect>(this, AbilityEffect);
//		if (NewEffect)
//		{
//			//Delay для таймера пока 0
//			NewEffect->InitObject(this, NAME_None);
//		}
//	}
//
//}

void AHomeWork22Character::SpawnDamageWidget_Implementation(float CurrentHealth, float Damage)
{

}

void AHomeWork22Character::DropCurrentWeaponChar()
{
	if (WeaponSwitchComponent->WeaponSlots.Num() > 0)
	{
		DropWeaponChar_Server(CurrentWeapon);
		//WeaponSwitchComponent->DropCurrentWeaponFromInventory(CurrentWeapon);
		//WeaponChangeCharacter(0);
	}
}

void AHomeWork22Character::DropWeaponChar_Server_Implementation(ADefaultWeapon* WeaponToDrop)
{
	DropWeaponChar_Multicast(WeaponToDrop);
	
}

void AHomeWork22Character::DropWeaponChar_Multicast_Implementation(ADefaultWeapon* WeaponToDrop)
{
	WeaponSwitchComponent->DropCurrentWeaponFromInventory(WeaponToDrop);
	WeaponChangeCharacterForDelegate_OnServer(0);
	//WeaponChangeCharacter(0);
}


//Network

void AHomeWork22Character::SetActorRotationByYaw_OnServer_Implementation(float _Yaw)
{
	SetActorRotationByYaw_MultiCast(_Yaw);
}

void AHomeWork22Character::SetActorRotationByYaw_MultiCast_Implementation(float _Yaw)
{
	if (Controller && !Controller->IsLocalPlayerController())
	{
		SetActorRotation(FQuat(FRotator(0.0f, _Yaw, 0.0f)));
	}

}


void AHomeWork22Character::SetMovementState_OnServer_Implementation(EMovementState NewState)
{
	SetMovementState_Multicast(NewState);
}

void AHomeWork22Character::SetMovementState_Multicast_Implementation(EMovementState NewState)
{
	MovementState = NewState;
	CharacterUpdate();

}


bool AHomeWork22Character::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	// Реплицируем наш объект.
	if (AbilityEffect) WroteSomething |= Channel->ReplicateSubobject(AbilityEffect, *Bunch, *RepFlags);

	return WroteSomething;

}

void AHomeWork22Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHomeWork22Character, MovementState);
	DOREPLIFETIME(AHomeWork22Character, CurrentWeapon);
	DOREPLIFETIME(AHomeWork22Character, RMBPressed);
	DOREPLIFETIME(AHomeWork22Character, PistolEquiped);
	DOREPLIFETIME(AHomeWork22Character, IsReloading);
	DOREPLIFETIME(AHomeWork22Character, ReloadCharAnim);
	DOREPLIFETIME(AHomeWork22Character, AbilityEffect);
}

void AHomeWork22Character::WeaponEquipAnimChar_Anim_Multicast_Implementation(UAnimMontage* AnimChangeMontage, float PlayRate)
{
	PlayAnimMontage(AnimChangeMontage, PlayRate);
}

void AHomeWork22Character::ChangeCharAimState_Multicast_Implementation(bool bIsAiming)
{
	RMBPressed = bIsAiming;
	ChangeMovementState();
	CharacterUpdate();
}

void AHomeWork22Character::ChangeCharAimState_Server_Implementation(bool bIsAiming)
{
	ChangeCharAimState_Multicast(bIsAiming);
}

void AHomeWork22Character::StartReloadChar_Multicast_Implementation(UAnimMontage* ReloadMontage, float ReloadTime)
{
	PlayAnimMontage(ReloadMontage, ReloadTime);
}

void AHomeWork22Character::ReloadInit_Server_Implementation()
{
	CurrentWeapon->InitReload();
}

void AHomeWork22Character::StopAllAnim_Server_Implementation()
{
	if (this->GetMesh()->GetAnimInstance())
	{
		this->GetMesh()->GetAnimInstance()->StopAllMontages(0.15f);
	}

	IsReloading = false;

	if (GetCurrentWeapon())
	{
		if (GetCurrentWeapon()->SkeletalMeshWeapon && GetCurrentWeapon()->SkeletalMeshWeapon->GetAnimInstance())
		{
			GetCurrentWeapon()->SkeletalMeshWeapon->GetAnimInstance()->StopAllMontages(0.15f);
			GetCurrentWeapon()->SkeletalMeshWeapon->GetAnimInstance()->StopSlotAnimation(0.15f);
		}

		if (GetCurrentWeapon()->ReloadSound)
		{
			if (GetCurrentWeapon()->ReloadSound->IsPlaying())
			{
				GetCurrentWeapon()->ReloadSound->Deactivate();
			}

		}
	}
}

void AHomeWork22Character::UpdateCharHealth_Multicast_Implementation(float Damage)
{
	CharHealthComp->ChangeCurrentHealth(Damage);
}

