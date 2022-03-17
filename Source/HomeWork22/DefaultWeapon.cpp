// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultWeapon.h"
#include "Components/ArrowComponent.h"
#include "Engine.h"
#include "SleeveActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "FunctionLibrary/Types.h"
#include "Net/UnrealNetwork.h"
#include "FunctionLibrary/StateEffect.h"
#include "WeaponSwitchActor.h"



// Sets default values
ADefaultWeapon::ADefaultWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Создаем компонент Сцены
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	//Rootcomponent наследуется от Actor.h
	RootComponent = SceneComponent;


	//Создали компонент скелет-мэша
	SkeletalMeshWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	//Задаем так, чтобы SkeletalMeshComponent не создавал Overlap Event'ов
	SkeletalMeshWeapon->SetGenerateOverlapEvents(false);
	//Потом выбираем профиль (параметры коллизии) "NoCollision"
	SkeletalMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	//Ну и наконец аттачим его к рутовому компоненту RootComponent, которым стал SceneComponent
	SkeletalMeshWeapon->SetupAttachment(RootComponent); 

	// Тоже самое для обычного мэш-компонента.
	StaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	StaticMeshWeapon->SetGenerateOverlapEvents(false);
	StaticMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshWeapon->SetupAttachment(RootComponent);

	//Стрелке не нужны пресеты коллизии
	ShootLocationArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ShootLocationArrow->SetupAttachment(StaticMeshWeapon);
	
	//NetWork
	SetReplicates(true);


}

// Called when the game starts or when spawned
void ADefaultWeapon::BeginPlay()
{
	Super::BeginPlay();
	WeaponInit();
	
}

// Called every frame
void ADefaultWeapon::Tick(float DeltaTime)
{	
	Super::Tick(DeltaTime);
	FireTick(DeltaTime);
	ReloadTick(DeltaTime);
	DispertionTick(DeltaTime);
	
}

void ADefaultWeapon::FireTick(float DeltaTime)
{
	if (GetWeaponRound()>0)
	{
		if (WeaponFiring && !WeaponReloading)
		{
			if (FireTime < 0.f)
			{
				Fire();
			}
			else
			{
				FireTime -= DeltaTime;
			}
		}
	
	}
	else
	{
		if (!WeaponReloading && CanWeaponReload())
		{
			InitReload();
		}
	}
	
}

void ADefaultWeapon::ReloadTick(float DeltaTime)
{
	if (WeaponReloading) 
	{
		if (ReloadTime <= 0.0f)
		{
			FinishReload();
		}
		else
		{
			ReloadTime -= DeltaTime;
		}
	}
}

void ADefaultWeapon::DispertionTick(float DeltaTime)
{
	if (!WeaponReloading)
	{
		if (!WeaponFiring)
		{
			if (ShouldReduceDispertion)
			{
				CurrentDispertion -= CurrentDispertionReduction;
			}
			else
			{
				CurrentDispertion += CurrentDispertionReduction;
			}
		}
		if (CurrentDispertion < CurrentDispertionMin)
		{
			CurrentDispertion = CurrentDispertionMin;
		}
		else
		{
			if ((CurrentDispertion > CurrentDispertionMax))
			{
				CurrentDispertion = CurrentDispertionMax;
			}

		}
	}
}

void ADefaultWeapon::WeaponInit()
{
	if (SkeletalMeshWeapon && !SkeletalMeshWeapon->SkeletalMesh)
	{
		SkeletalMeshWeapon->DestroyComponent(true);
	}
	if (StaticMeshWeapon && !StaticMeshWeapon->GetStaticMesh())
	{
		StaticMeshWeapon->DestroyComponent();
	}
	//AdditionalWeaponInfo.Round = WeaponSetting.MaxRound;
}

bool ADefaultWeapon::CheckWeaponCanFire()
{
	return !BlockFire;
}

FProjectileInfo ADefaultWeapon::GetProjectile()
{
	return WeaponSetting.ProjectileSetting;
}

//Огонь из оружия
void ADefaultWeapon::Fire()
{
	// On Server

	if (bIsEquiped)
	{

		FireTime = WeaponSetting.RateOfFire;
		ChangeDispertionByShot();
		if (WeaponSetting.ProjectileSetting.Projectile)
		{

			if (ShootLocationArrow)
			{
				FVector SpawnLocation = ShootLocationArrow->GetComponentLocation();
				FRotator SpawnRotation = ShootLocationArrow->GetComponentRotation();
				FProjectileInfo ProjectileInfo;
				ProjectileInfo = GetProjectile();

				//FQuat NewQuat(0,0,0.7,0.7);
				//NewQuat.Rotator(ShootLocationArrow->GetForwardVector());
				FTransform NewTransform;
				//NewTransform.SetRotation(NewQuat);
				NewTransform.SetRotation(FQuat(FRotator(0.0f, UKismetMathLibrary::FindLookAtRotation(ShootLocationArrow->GetComponentLocation(), ShootEndLocation).Yaw, 0.0f)));
				NewTransform.SetLocation(ShootLocationArrow->GetComponentLocation());
				NewTransform.SetScale3D(ShootLocationArrow->GetComponentTransform().GetScale3D());

				//network
				if (WeaponSetting.SoundFireWeapon)
				{
					SpawnMuzzleSound_Multicast(WeaponSetting.SoundFireWeapon, ShootLocationArrow->GetComponentLocation());
				}
				if (WeaponSetting.EffectFireWeapon)
				{
					SpawnMuzzleEmitter_Multicast(WeaponSetting.EffectFireWeapon, NewTransform);
				}
				

				int8 NumberOfProjectile = GetBulletPerShot();
				for (int i = 0; i < NumberOfProjectile; i++)
				{
					if (ProjectileInfo.Projectile) // проверка наличия выборанного патрона в BP weapon
					{
						FActorSpawnParameters SpawnParams;
						SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
						SpawnParams.Owner = GetOwner();
						SpawnParams.Instigator = GetInstigator();

						FVector Dir = GetFireEndLocation() - SpawnLocation; //Получаем направление
						Dir.Normalize(); // Приводим её к нормали

						FMatrix myMatrix(Dir, FVector(0, 1, 0), FVector(0, 0, 1), FVector::ZeroVector);
						SpawnRotation = myMatrix.Rotator();


						//Функция, которая переопределена в BluePrint'е оружия
						/*ProjectileRespawn(ProjectileInfo.Projectile,
							ProjectileInfo.ProjectileSpeed,
							SpawnLocation, SpawnRotation,
							ProjectileInfo.ProjectileLifeTime,
							ProjectileInfo);*/

						ADefaultProjectile* myProjectile = Cast<ADefaultProjectile>(GetWorld()->SpawnActor(ProjectileInfo.Projectile,&SpawnLocation,&SpawnRotation,SpawnParams));
						if (myProjectile)
						{
							myProjectile->InitProjectile(ProjectileInfo, Dir);
						}


					}
				}
				
			}
		
			if (WeaponSetting.FireGunAnim && AdditionalWeaponInfo.Round != 0)
			{
				AnimWeapon_Multicast(WeaponSetting.FireGunAnim);	
			}



		}
		//LineTraceLogic
		else
		{
			//Linetrace
			FHitResult CursorTraceHitResult; //LineTrace для курсора
			FHitResult TraceHitResult; // LineTrace для луча

			FCollisionQueryParams myParam;
			myParam.bReturnPhysicalMaterial = true;

			APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

			//myController->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, HitResult);
			myController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery3, true, CursorTraceHitResult);

			bool bHit = GetWorld()->LineTraceSingleByChannel(TraceHitResult, ShootLocationArrow->GetComponentLocation(),
				ShootLocationArrow->GetComponentLocation() + (ShootLocationArrow->GetComponentLocation() - ShootEndLocation).GetSafeNormal() * -WeaponSetting.TraceDistance,
				ECollisionChannel::ECC_GameTraceChannel1, myParam);

			DrawDebugLine(GetWorld(), ShootLocationArrow->GetComponentLocation(),
				ShootLocationArrow->GetForwardVector().GetSafeNormal() * WeaponSetting.TraceDistance + ShootLocationArrow->GetComponentLocation(), FColor::Red, false, 5);


			FTransform NewTransform;
			//NewTransform.SetRotation(NewQuat);
			NewTransform.SetRotation(FQuat(FRotator(0.0f, UKismetMathLibrary::FindLookAtRotation(ShootLocationArrow->GetComponentLocation(), ShootEndLocation).Yaw, 0.0f)));
			NewTransform.SetLocation(ShootLocationArrow->GetComponentLocation());
			NewTransform.SetScale3D(ShootLocationArrow->GetComponentTransform().GetScale3D());

			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSetting.SoundFireWeapon, ShootLocationArrow->GetComponentLocation());
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponSetting.EffectFireWeapon, NewTransform);


			if (TraceHitResult.GetActor() && TraceHitResult.PhysMaterial != nullptr)
			{
				EPhysicalSurface TraceHitSurface = UGameplayStatics::GetSurfaceType(TraceHitResult);

				if (WeaponSetting.ProjectileSetting.HitDecals.Contains(TraceHitSurface))
				{
					UMaterialInterface* myMaterial = WeaponSetting.ProjectileSetting.HitDecals[TraceHitSurface];


					if (myMaterial && TraceHitResult.GetComponent())
					{

						UDecalComponent* myDecal = UGameplayStatics::SpawnDecalAttached(myMaterial, FVector(5.0f), TraceHitResult.GetComponent(),
							NAME_None, TraceHitResult.ImpactPoint,
							TraceHitResult.Normal.Rotation(), EAttachLocation::KeepWorldPosition, 5.0f);

						if (myDecal)
						{
							myDecal->SetFadeScreenSize(0);
						}

					}

				}
				if (WeaponSetting.ProjectileSetting.HitFXs.Contains(TraceHitSurface))
				{
					UParticleSystem* TraceHitParticleSystem = WeaponSetting.ProjectileSetting.HitFXs[TraceHitSurface];
					if (TraceHitParticleSystem)
					{
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceHitParticleSystem,
							FTransform(TraceHitResult.Normal.Rotation(),
								TraceHitResult.ImpactPoint, FVector(1.0f)));

					}
				}

				
			UTypes::AddEffectBySurfaceType(TraceHitResult.GetActor(), TraceHitResult.BoneName, WeaponSetting.ProjectileSetting.Effect, TraceHitSurface);
			}
		


			if (WeaponSetting.FireGunAnim && AdditionalWeaponInfo.Round != 0)
			{
				SkeletalMeshWeapon->PlayAnimation(WeaponSetting.FireGunAnim, false);
			}

			UGameplayStatics::ApplyPointDamage(TraceHitResult.GetActor(), WeaponSetting.WeaponDamage, TraceHitResult.TraceStart, TraceHitResult, GetInstigatorController(), this, NULL);

		}
		AdditionalWeaponInfo.Round--;
		if (GetOwner())
		{

			UWeaponSwitchActor* MyInv = Cast<UWeaponSwitchActor>(GetOwner()->GetComponentByClass(UWeaponSwitchActor::StaticClass()));
			if (MyInv)
			{
				MyInv->FOnFireInInventory();
			}
		}



		if (WeaponSetting.HipAnimCharFire)
		{
			OnFire.Broadcast(true);
		}
		else 
		{
			OnFire.Broadcast(false);

		}
		//Shells
		if (SkeletalMeshWeapon)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = GetInstigator();
			FVector ZeroVector = SkeletalMeshWeapon->GetSocketLocation(FName("SleeveSocket"));
			FRotator ZeroRotator = SkeletalMeshWeapon->GetSocketRotation(FName("SleeveSocket"));

			ASleeveActor* mySleeve = Cast<ASleeveActor>(GetWorld()->SpawnActor(WeaponSetting.SleeveActor, &ZeroVector, &ZeroRotator, SpawnParams));
			if (mySleeve)
			{
				FAttachmentTransformRules myRules(EAttachmentRule::SnapToTarget, false);
				mySleeve->StaticMeshSleeve->AddImpulse(ZeroRotator.Vector() * WeaponSetting.SleeveImpulseValue);			
			}

		}

	}
}

void ADefaultWeapon::UpdateStateWeapon_OnServer_Implementation(EMovementState NewMovementState)
{
	//to do Dispertion

	BlockFire = false;

	switch (NewMovementState)
	{
	case EMovementState::Aim_State:
		CurrentDispertionMax = WeaponSetting.Dispersion.AimWalk_StateDispersionAimMax;
		CurrentDispertionMin = WeaponSetting.Dispersion.AimWalk_StateDispersionAimMin;
		CurrentDispertionRecoil = WeaponSetting.Dispersion.AimWalk_StateDispersionAimRecoil;
		CurrentDispertionReduction = WeaponSetting.Dispersion.AimWalk_StateDispersionAimReduction;
		break;
	case EMovementState::Walk_State:
		CurrentDispertionMax = WeaponSetting.Dispersion.Walk_StateDispersionAimMax;
		CurrentDispertionMin = WeaponSetting.Dispersion.Walk_StateDispersionAimMin;
		CurrentDispertionRecoil = WeaponSetting.Dispersion.Walk_StateDispersionAimRecoil;
		CurrentDispertionReduction = WeaponSetting.Dispersion.Walk_StateDispersionAimReduction;
		break;
	case EMovementState::Run_State:
		CurrentDispertionMax = WeaponSetting.Dispersion.Run_StateDispersionAimMax;
		CurrentDispertionMin = WeaponSetting.Dispersion.Run_StateDispersionAimMin;
		CurrentDispertionRecoil = WeaponSetting.Dispersion.Run_StateDispersionAimRecoil;
		CurrentDispertionReduction = WeaponSetting.Dispersion.Run_StateDispersionAimReduction;
		break;
	case EMovementState::Sprint_State:
		BlockFire = true;
		SetWeaponStateFire_OnServer(false); //set fire trigget to false
		//Block Fire
		break;
	default:
		break;
	}
}


int32 ADefaultWeapon::GetWeaponRound()
{
	return AdditionalWeaponInfo.Round;
}

void ADefaultWeapon::InitReload()
{
	if (bIsEquiped)
	{

		WeaponReloading = true;
		ReloadTime = WeaponSetting.ReloadTime;
		ReloadSound = nullptr;
		if (WeaponSetting.SoundReload)
		{
			PlayReloadSoundWeapon_Client(WeaponSetting.SoundReload);			
		}
		
		if (WeaponSetting.ReloadGunAnim)
		{
			SkeletalMeshWeapon->SetPlayRate(WeaponSetting.ReloadGunAnim->GetMaxCurrentTime() / WeaponSetting.ReloadTime);
			SkeletalMeshWeapon->PlayAnimation(WeaponSetting.ReloadGunAnim, false);
		}


		OnWeaponReloadStart.Broadcast();
		//добавить анимацию
	}

}

void ADefaultWeapon::FinishReload()
{
	WeaponReloading = false;
	
	AdditionalWeaponInfo.Round = GetAvailableAmmo();

	OnWeaponReloadEnd.Broadcast();

	if (GetOwner())
	{
		UWeaponSwitchActor* MyInv = Cast<UWeaponSwitchActor>(GetOwner()->GetComponentByClass(UWeaponSwitchActor::StaticClass()));
		if (MyInv)
		{
			MyInv->ReloadInInventory();
		}
	}


}

FVector ADefaultWeapon::GetFireEndLocation() const
{
	bool bShootDirection = false;
	FVector EndLocation = FVector(0.f);

	//FVector tmpV = (ShootLocationArrow->GetComponentLocation() - ShootEndLocation);

	if (byBarrel) 
	{
		EndLocation =  
		ShootLocationArrow->GetComponentLocation() + ApplyDispertionToShoot(ShootLocationArrow->GetComponentLocation() - ShootEndLocation).GetSafeNormal() * -20000.0f;
		
	}
	else
	{
		EndLocation = ShootLocationArrow->GetComponentLocation() + ApplyDispertionToShoot(ShootLocationArrow->GetComponentLocation() - ShootEndLocation).GetSafeNormal() * -20000.0f;
		//EndLocation = ShootLocationArrow->GetComponentLocation() + ApplyDispertionToShoot(ShootLocationArrow->GetForwardVector())*20000.0f;
		DrawDebugCone(GetWorld(), ShootLocationArrow->GetComponentLocation(), -(ShootLocationArrow->GetComponentLocation() - ShootEndLocation), (ShootLocationArrow->GetComponentLocation() - ShootEndLocation).Size(), GetCurrentDispertion() * PI / 180.f, GetCurrentDispertion() * PI / 180.f, 32, FColor::Emerald, false, .1f, (uint8)'\000', 1.0f);
	}

	return EndLocation;
}

FVector ADefaultWeapon::ApplyDispertionToShoot(FVector DispertionShoot) const
{
	return FMath::VRandCone(DispertionShoot, GetCurrentDispertion() * PI / 180.f);
}


float ADefaultWeapon::GetCurrentDispertion() const
{
	return CurrentDispertion;
}


void ADefaultWeapon::ChangeDispertionByShot()
{
	CurrentDispertion = CurrentDispertion + CurrentDispertionRecoil;
}



int32 ADefaultWeapon::GetBulletPerShot()
{
	return WeaponSetting.BulletPerShot;
}

bool ADefaultWeapon::CanWeaponReload()
{

	if (GetOwner())
	{

		UWeaponSwitchActor* MyInv = Cast<UWeaponSwitchActor>(GetOwner()->GetComponentByClass(UWeaponSwitchActor::StaticClass()));
		if (MyInv)
		{
			if (!MyInv->CheckAmmoForWeapon(WeaponSetting.WeaponType))
			{
				return false;
			} 
		}
	}


	return true;
}

int32 ADefaultWeapon::GetAvailableAmmo()
{
	int8 ResultAmmo = AdditionalWeaponInfo.Round;
	if (GetOwner())
	{
		UWeaponSwitchActor* MyInv = Cast<UWeaponSwitchActor>(GetOwner()->GetComponentByClass(UWeaponSwitchActor::StaticClass()));
		if (!MyInv)
		{
				return 0;
		}
		else
		{
		ResultAmmo = MyInv->WeaponSlots[MyInv->GetCurrentIndex()].AdditionalInfo.Round;

		int8 AmmoIndex = 0;
		if (MyInv->GetAmmoIndexByType(WeaponSetting.WeaponType)!=-1)
		{
			AmmoIndex = MyInv->GetAmmoIndexByType(WeaponSetting.WeaponType);

			if (ResultAmmo + MyInv->AmmoSlots[AmmoIndex].Count >= WeaponSetting.MaxRound)
			{

				MyInv->AmmoSlots[AmmoIndex].Count = MyInv->AmmoSlots[AmmoIndex].Count
					- (WeaponSetting.MaxRound - ResultAmmo);


				//Патроны в обойме класса Оружия
				ResultAmmo += WeaponSetting.MaxRound - ResultAmmo;

				//Патроны в обойме класса инвентаря
				MyInv->WeaponSlots[MyInv->GetCurrentIndex()].AdditionalInfo.Round = ResultAmmo;

			}
			else
			{
				//Патроны в обойме класса Оружия
				ResultAmmo += MyInv->AmmoSlots[AmmoIndex].Count;

				//Патроны в обойме класса инвентаря

				MyInv->WeaponSlots[MyInv->GetCurrentIndex()].AdditionalInfo.Round = ResultAmmo;
				MyInv->AmmoSlots[AmmoIndex].Count = 0;
			}


		}
		else
		{

			return 0;

		}

		}

	}
	return ResultAmmo;
}



void ADefaultWeapon::SetWeaponStateFire_OnServer_Implementation(bool bIsFire)
{
	if (CheckWeaponCanFire())
	{
		WeaponFiring = bIsFire;
	}
	else
	{
		WeaponFiring = false;
		FireTime = 0.01f;
	}
}

FWeaponInfo ADefaultWeapon::GetWeaponSetting()
{
	return WeaponSetting;
}

void ADefaultWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADefaultWeapon, AdditionalWeaponInfo);
	DOREPLIFETIME(ADefaultWeapon, WeaponReloading);
}

void ADefaultWeapon::UpdateWeaponByCharacterMovement_OnServer_Implementation(FVector NewShootEndLocation, bool NewShouldReduceDispertion)
{
	ShootEndLocation = NewShootEndLocation;
	ShouldReduceDispertion = NewShouldReduceDispertion;
}

void ADefaultWeapon::SpawnMuzzleEmitter_Multicast_Implementation(UParticleSystem* MuzzleFire, FTransform MuzzleLocation)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFire, MuzzleLocation);
}

void ADefaultWeapon::SpawnMuzzleSound_Multicast_Implementation(USoundBase* MuzzleSound, FVector MuzzleLocation)
{

	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), MuzzleSound, MuzzleLocation);
}

void ADefaultWeapon::AnimWeapon_Multicast_Implementation(UAnimationAsset* FireGunAnim)
{
	SkeletalMeshWeapon->PlayAnimation(FireGunAnim, false);
}


void ADefaultWeapon::PlayReloadSoundWeapon_Client_Implementation(USoundBase* ReloadSoundWeapon)
{
	ReloadSound = UGameplayStatics::SpawnSound2D(GetWorld(), ReloadSoundWeapon, 1.0f, 1.0f, 0.0f, nullptr, false, false);
}
