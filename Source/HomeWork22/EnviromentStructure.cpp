// Fill out your copyright notice in the Description page of Project Settings.


#include "EnviromentStructure.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "FunctionLibrary/Types.h"



// Sets default values
AEnviromentStructure::AEnviromentStructure()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	RootComponent = SceneComponent;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(SceneComponent);

	DestructibleMesh = CreateDefaultSubobject<UDestructibleComponent>("DestructibleMesh");
	DestructibleMesh->SetupAttachment(SceneComponent);

	SetReplicates(true);

}

// Called when the game starts or when spawned
void AEnviromentStructure::BeginPlay()
{
	Super::BeginPlay();
	
	if (DestructibleMesh && !StaticMesh->GetStaticMesh())
	{
		StaticMesh->DestroyComponent(true);
	}
	if (StaticMesh && !DestructibleMesh->GetDestructibleMesh())
	{
		DestructibleMesh->DestroyComponent(true);
	}

}

// Called every frame
void AEnviromentStructure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
//	GEngine->AddOnScreenDebugMessage(-1,0,FColor::Orange, this->GetActorLocation().ToString());
}

EPhysicalSurface AEnviromentStructure::GetSurfaceType()
{
	EPhysicalSurface Result = EPhysicalSurface::SurfaceType_Default;
	UStaticMeshComponent* myMesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (myMesh)
	{
		UMaterialInterface* myMaterial = myMesh->GetMaterial(0);
		if (myMaterial)
		{
			Result = myMaterial->GetPhysicalMaterial()->SurfaceType;
		}
	}

	return Result;
}

TArray<UStateEffect*> AEnviromentStructure::GetAllCurrentEffects()
{
	return Effects;
}

void AEnviromentStructure::RemoveEffect(UStateEffect* RemoveEffect)
{
	Effects.Remove(RemoveEffect);
}

void AEnviromentStructure::AddEffect(UStateEffect* NewEffect)
{
	Effects.Add(NewEffect);
}

void AEnviromentStructure::Explosion(float BaseDamageValue, float Radius, UParticleSystem* ExplosionParticle, TSubclassOf<UStateEffect> ExpEffect, USoundBase* ExplosionSound)
{
	if (!bIsExploded)
	{
		FCollisionShape MyColSphere = FCollisionShape::MakeSphere(Radius);

		TArray<FHitResult> HitResults;
		FCollisionQueryParams TraceParam;
		TraceParam.bReturnPhysicalMaterial = true;
		bool DebugSphereHit = GetWorld()->SweepMultiByChannel(HitResults, this->GetActorLocation(), this->GetActorLocation() + FVector(0, 0, 10), FQuat::Identity,ECollisionChannel::ECC_Visibility, MyColSphere, TraceParam);

		TArray<AActor*> IgnoredActor;
		IgnoredActor.Add(this);
		ExpolisionDamage_Multicast(BaseDamageValue, BaseDamageValue * 0.4, this->GetActorLocation() + FVector(0, 0, 10), Radius * 0.4, Radius, 1, nullptr, IgnoredActor, this, nullptr, ECollisionChannel::ECC_Visibility);

		//bool Explosions = UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), BaseDamageValue, BaseDamageValue *0.4, this->GetActorLocation()+FVector(0,0,10),Radius*0.4,Radius,1,nullptr,IgnoredActor,this,nullptr, ECollisionChannel::ECC_Visibility);


		for (auto HitRes : HitResults)
		{
			EPhysicalSurface mySurfaceType = UGameplayStatics::GetSurfaceType(HitRes);
		//	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green,HitRes.GetActor()->GetName());

			if (HitRes.GetActor() && ExpEffect && HitRes.PhysMaterial.IsValid() && mySurfaceType)
			{
			//	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, TEXT("True for = ") + HitRes.GetActor()->GetName());
				UTypes::AddEffectBySurfaceType(HitRes.GetActor(), HitRes.BoneName, ExpEffect, mySurfaceType);
			}
		}
		

		FTransform NewTransform;
		NewTransform = this->GetActorTransform();

		if (ExplosionParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, NewTransform);
		}
		if (ExplosionSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, NewTransform.GetLocation());
		}

		if (bDrawDebug)
		{
			DrawDebugSphere(GetWorld(), this->GetActorLocation(), Radius * 0.4, 20, FColor::Green, false, 10);
			DrawDebugSphere(GetWorld(), this->GetActorLocation(), Radius, 20, FColor::Red, false, 10);
			DrawDebugPoint(GetWorld(), this->GetActorLocation(), 30, FColor::Purple, false, 60);
			DrawDebugPoint(GetWorld(), this->GetActorLocation() + FVector(0, 0, 1), 30, FColor::Red, false, 60);
		}

		if (StaticMesh && StaticMesh->GetStaticMesh())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, TEXT("Mesh?"));
			this->Destroy();
		}

		if (DestructibleMesh)
		{
			GetWorld()->GetTimerManager().SetTimer(myTimerToDestroy, this, &AEnviromentStructure::DestroyActor1, 7, false);

			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("DestructibleMesh"));
			DestructibleMesh->ApplyRadiusDamage(100,this->GetActorLocation(),Radius,20000,false);
		}

		bIsExploded = true;
	}
}

void AEnviromentStructure::DestroyActor1()
{
	this->Destroy();
}

void AEnviromentStructure::ExpolisionDamage_Multicast_Implementation(float BaseDamage, float MinimumDamage, const FVector& Origin, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor* DamageCauser, AController* InstigatedByController, ECollisionChannel DamagePreventionChannel)
{
	bool Explosions = UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), BaseDamage, MinimumDamage, Origin + FVector(0, 0, 10), DamageInnerRadius * 0.4, DamageOuterRadius, 1, nullptr, IgnoreActors, DamageCauser, InstigatedByController, DamagePreventionChannel);
}


