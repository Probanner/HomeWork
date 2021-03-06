// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary/StateEffect.h"
#include "HeathComponent.h"
#include "FunctionLibrary/IGameActor.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "Kismet/KismetSystemLibrary.h"

bool UStateEffect::InitObject(AActor* Actor, FName ActorBoneName)
{
	myActor = Actor;
	IIGameActor* myInterface = Cast<IIGameActor>(myActor);
	if (myInterface)
	{
		myInterface->AddEffect(this);
	}
	
	return true;
}

void UStateEffect::ExecuteEffect(float DeltaTime)
{
	
}	

void UStateEffect::DestroyObject()
{
	
	IIGameActor* myInterface = Cast<IIGameActor>(myActor);
	if (myInterface)
	{
		myInterface->RemoveEffect(this);
	}

	if (this && this->IsValidLowLevel())
	{
		myActor = nullptr;
		this->ConditionalBeginDestroy();
	}
}


bool UStateEffect_ExecuteOnce::InitObject(AActor* Actor,FName ActorBoneName)
{
	Super::InitObject(Actor, ActorBoneName);
	ExecuteOnce();
	return true;
}

void UStateEffect_ExecuteOnce::DestroyObject()
{
	Super::DestroyObject();
}

void UStateEffect_ExecuteOnce::ExecuteOnce()
{
	//FX
	if (myActor)
	{
		UHeathComponent* myHealthComp = Cast<UHeathComponent>(myActor->GetComponentByClass(UHeathComponent::StaticClass()));

		if (myHealthComp)
		{
			myHealthComp->ChangeCurrentHealth(Power);
		}

	}
	
	DestroyObject();
}



bool UStateEffect_ExecuteTimer::InitObject(AActor* Actor, FName ActorBoneName)
{
	Super::InitObject(Actor, ActorBoneName);
	
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(StateEffect_ExecuteTimer, this, &UStateEffect_ExecuteTimer::DestroyObject, Timer, false);

		GetWorld()->GetTimerManager().SetTimer(StateEffect_EffectTimer, this, &UStateEffect_ExecuteTimer::ExecuteByTimer, TimerRate, true, 0.0f);

		GetWorld()->GetTimerManager().SetTimer(TimerEffectFade, this, &UStateEffect_ExecuteTimer::EffectScaleDecrease, EffectFadeRate, true, 0);
	}

	float SecondsForScale = Timer - (Timer * 0.8f);
	float TimesToScale = SecondsForScale / EffectFadeRate;

	if (ParticleForEffect)
	{
		USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(myActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));

		if (SkeletalMesh)
		{
			if (SkeletalMesh->GetBoneIndex(ActorBoneName) != INDEX_NONE)
			{
				SpawnEmitterAttached_Multicast(ParticleForEffect, SkeletalMesh, ActorBoneName, FVector(0), FRotator(-90, 0, 0), EAttachLocation::SnapToTarget, false);
				//ParticleEmitter = UGameplayStatics::SpawnEmitterAttached(ParticleForEffect, SkeletalMesh, ActorBoneName, FVector(0), FRotator(-90,0,0), EAttachLocation::SnapToTarget, false);

				ScaleOfScaleVector = ParticleEmitter->GetRelativeScale3D().X / TimesToScale;
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("NE NASHEL KOST' ! "));
				FName BoneNameToAttach;
				FVector LocationToAttach = FVector(0, 0, 40);

				SpawnEmitterAttached_Multicast(ParticleForEffect, myActor->GetRootComponent(), BoneNameToAttach, LocationToAttach, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);

				//ParticleEmitter = UGameplayStatics::SpawnEmitterAttached(ParticleForEffect, myActor->GetRootComponent(), BoneNameToAttach, LocationToAttach, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);

				ScaleOfScaleVector = ParticleEmitter->GetRelativeScale3D().X / TimesToScale;
			}
		}
		else
		{
			FName BoneNameToAttach;
			FVector LocationToAttach = FVector(0, 0, 40);

			SpawnEmitterAttached_Multicast(ParticleForEffect, myActor->GetRootComponent(), BoneNameToAttach, LocationToAttach, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);

			//ParticleEmitter = UGameplayStatics::SpawnEmitterAttached(ParticleForEffect, myActor->GetRootComponent(), BoneNameToAttach, LocationToAttach, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);

			ScaleOfScaleVector = ParticleEmitter->GetRelativeScale3D().X / TimesToScale;
		}
		
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("ScaleOfScaleVector = ") + FString::SanitizeFloat(ScaleOfScaleVector));
	}
	return true;
}

void UStateEffect_ExecuteTimer::DestroyObject()
{
	if (GetWorld())
	{
		if (ParticleEmitter)
		{
			RemoveEffect_Multicast();
		}
	}
	
	Super::DestroyObject();
}

void UStateEffect_ExecuteTimer::RemoveEffect_Multicast_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerEffectFade);
	ParticleEmitter->DestroyComponent();
	ParticleEmitter = nullptr;
	GetWorld()->GetTimerManager().ClearTimer(StateEffect_EffectTimer);
}


void UStateEffect_ExecuteTimer::ExecuteByTimer()
{
	if (myActor)
	{
		TimerDoneTimes++;
		UHeathComponent* myHealthComp = Cast<UHeathComponent>(myActor->GetComponentByClass(UHeathComponent::StaticClass()));

		if (myHealthComp)
		{
			myHealthComp->ChangeCurrentHealth(Power);
		}

		if (myHealthComp)
		{
			if (myHealthComp->GetCurrentHealth() <= 0)
			{
				UStateEffect_ExecuteTimer::DestroyObject();
			}
		}
		
		
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, ParticleEmitter->GetRelativeLocation().ToString());
		
	}
}

void UStateEffect_ExecuteTimer::RefreshTimerDamageTick()
{
	if (GetOuter()->GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(StateEffect_ExecuteTimer, this, &UStateEffect_ExecuteTimer::DestroyObject, Timer, false);
		GetWorld()->GetTimerManager().SetTimer(TimerEffectFade, this, &UStateEffect_ExecuteTimer::EffectScaleDecrease, EffectFadeRate, true, 0);
		Secs = 0;
	}
	
}

void UStateEffect_ExecuteTimer::EffectScaleDecrease()
{
	Secs += EffectFadeRate;
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("Secs = ") + FString::SanitizeFloat(Secs));
	if (ParticleEmitter)
	{
		if (EffectScaleDecreaseAtEnd && !EffectScaleIncreaseAtEnd && (Secs >= Timer * 0.8f) && ParticleEmitter->GetRelativeScale3D().X > 0.0f)
		{
			if (GEngine)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("UStateEffect_ExecuteTimer::EffectScaleDecrease() = ") + ParticleEmitter->GetRelativeScale3D().ToString());
				//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("Secs = ") + FString::SanitizeFloat(Secs));
			}
			SetEffectScale_Multicast(ParticleEmitter->GetRelativeScale3D() - FVector(ScaleOfScaleVector, ScaleOfScaleVector, ScaleOfScaleVector));
			//ParticleEmitter->SetRelativeScale3D(ParticleEmitter->GetRelativeScale3D() - FVector(ScaleOfScaleVector, ScaleOfScaleVector, ScaleOfScaleVector));
		}
		else if (!EffectScaleDecreaseAtEnd && EffectScaleIncreaseAtEnd)
		{
			SetEffectScale_Multicast(ParticleEmitter->GetRelativeScale3D() + FVector(ScaleOfScaleVector, ScaleOfScaleVector, ScaleOfScaleVector));
			//ParticleEmitter->SetRelativeScale3D(ParticleEmitter->GetRelativeScale3D() + FVector(ScaleOfScaleVector, ScaleOfScaleVector, ScaleOfScaleVector));
		}
	}
	
}

void UStateEffect_ExecuteTimer::SetEffectScale_Multicast_Implementation(FVector NewParticle)
{
	ParticleEmitter->SetRelativeScale3D(NewParticle);
}


void UStateEffect_ExecuteTimer::SpawnEmitterAttached_Multicast_Implementation(UParticleSystem* ParticleForStateEffect, USceneComponent* CompToAttach, FName BoneName, FVector Location, FRotator Rotation, EAttachLocation::Type LocationType, bool bAutoDestroy)
{
	ParticleEmitter = UGameplayStatics::SpawnEmitterAttached(ParticleForStateEffect, CompToAttach, BoneName, Location, Rotation, LocationType, bAutoDestroy);
}

void UStateEffect_ExecuteTimer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStateEffect_ExecuteTimer, ParticleEmitter);
	DOREPLIFETIME(UStateEffect_ExecuteTimer, ParticleForEffect);
}



//RPC ??????? ? UObject
bool UStateEffect_ExecuteTimer::CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack)
{
	if (!GetOuter()) return false;
	UNetDriver* NetDriver = Cast<AActor>(GetOuter())->GetNetDriver();
	if (!NetDriver) return false;

	NetDriver->ProcessRemoteFunction(Cast<AActor>(GetOuter()), Function, Parms, OutParms, Stack, this);

	return true;
}

int32 UStateEffect_ExecuteTimer::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	return (GetOuter() ? GetOuter()->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local);
}
