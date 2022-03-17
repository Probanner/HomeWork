// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultProjectile.h"
#include "Rendering/SkeletalMeshLODModel.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Engine.h"
#include "Net/UnrealNetwork.h"
#include <cmath>

// Sets default values
ADefaultProjectile::ADefaultProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BulletCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));

	BulletCollisionSphere->SetSphereRadius(5.0f);
	BulletCollisionSphere->OnComponentHit.AddDynamic(this, &ADefaultProjectile::BulletCollisionSphereHit);
	BulletCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ADefaultProjectile::BulletCollisionSphereBeginOverlap);
	BulletCollisionSphere->OnComponentEndOverlap.AddDynamic(this, &ADefaultProjectile::BulletCollisionSphereEndOverlap);

	BulletCollisionSphere->bReturnMaterialOnMove = true; //hit event return physMaterial
	BulletCollisionSphere->SetCanEverAffectNavigation(false); //collision doesn't affect navigation (P keybord on editor)

	RootComponent = BulletCollisionSphere;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshProjectileComponent"));
	BulletMesh->SetupAttachment(RootComponent);
	BulletMesh->SetCanEverAffectNavigation(false);

	BulletFx = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BulletFxParticleSystemComponent"));
	BulletFx->SetupAttachment(RootComponent);

	BulletProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	BulletProjectileMovement->bAutoActivate = false;
	BulletProjectileMovement->UpdatedComponent = RootComponent; // копмонент, который двигает система
	BulletProjectileMovement->bRotationFollowsVelocity = true;
	BulletProjectileMovement->bShouldBounce = true;
	BulletProjectileMovement->Activate(false);
	//Network
	SetReplicates(true);
}



// Called when the game starts or when spawned
void ADefaultProjectile::BeginPlay()
{
	Super::BeginPlay();
	SpawnLocation = GetActorLocation();

	FCollisionQueryParams NewParams;
	NewParams.bTraceComplex = true;
	FCollisionResponseParams NewRespParams;
	BulletCollisionSphere->InitSweepCollisionParams(NewParams, NewRespParams);
	BulletCollisionSphere->bTraceComplexOnMove = true;
}

// Called every frame
void ADefaultProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADefaultProjectile::BulletCollisionSphereHit(class UPrimitiveComponent* HitComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1,3,FColor::Red, Hit.Component->GetFName().ToString());
	if (OtherActor && Hit.PhysMaterial.IsValid())
	{

		EPhysicalSurface mySurfaceType = UGameplayStatics::GetSurfaceType(Hit);


		if (ProjectileSetting.HitDecals.Contains(mySurfaceType))
		{
			UMaterialInterface* myMaterial = ProjectileSetting.HitDecals[mySurfaceType];

			//Net Multicast
			SpawnHitDecal_Multicast(myMaterial, OtherComponent, Hit, ProjectileSetting.bIsLikeBomb);

		}
		if (ProjectileSetting.HitFXs.Contains(mySurfaceType))
		{		
			//Net Multicast
			SpawnHitFX_Multicast(ProjectileSetting.HitFXs[mySurfaceType],Hit);
		}

		TArray<USoundBase*> SoundsOfSurf;

		for (auto SurfSound : ProjectileSetting.SoundsOfHits)
		{
			if (SurfSound.Value == mySurfaceType)
			{
				SoundsOfSurf.Add(SurfSound.Key);
			}
		}
		if (SoundsOfSurf.Num() > 0)
		{
			int RandomSoundIndex = FMath::RandRange(0, SoundsOfSurf.Num() - 1);
			SpawnHitSound_Multicast(SoundsOfSurf[RandomSoundIndex],Hit);
		}


		//damage drop per distance
		if (!ProjectileSetting.bIsLikeBomb)
		{
			float ImpactDistanceTemp = (SpawnLocation - Hit.ImpactPoint).Size();
			float Delitel = ((ImpactDistanceTemp / 100) - ProjectileSetting.MaximumDamageDistance) / ProjectileSetting.DamageDropDistance;

			if (ImpactDistanceTemp / 100 > ProjectileSetting.MaximumDamageDistance && Delitel > 1)
			{
				ProjectileSetting.ProjectileDamage = ProjectileSetting.ProjectileDamage / (ProjectileSetting.DamageDropCoef * Delitel);
			}

		}

		//if grenade launcher
		if (ProjectileSetting.bIsLikeBomb)
		{

			//TArray<FHitResult> Outhits;
			FCollisionShape MyColSphere = FCollisionShape::MakeSphere(ProjectileSetting.BombRadius);
			// DebugShpere

			if (ProjectileSetting.BDrawDebugSphere)
			{
				DrawDebugSphere(GetWorld(), Hit.ImpactPoint, ProjectileSetting.BombRadius, 10, FColor::Red, false, 5);
			}

			TArray<AActor*> IgnoredActor;
			IgnoredActor.Add(this);

			bool explosion = UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), ProjectileSetting.ProjectileDamage, ProjectileSetting.ProjectileDamage * 0.1f,
				Hit.ImpactPoint + FVector(0, 0, 50), ProjectileSetting.BombRadius * 0.2f, ProjectileSetting.BombRadius, 1, NULL, IgnoredActor, this, nullptr, ECC_Visibility);

			TArray<FHitResult> HitResults;
			FCollisionQueryParams TraceParam;
			TraceParam.bReturnPhysicalMaterial = true;
			bool DebugSphereHit = GetWorld()->SweepMultiByChannel(HitResults, this->GetActorLocation(), this->GetActorLocation() + FVector(0, 0, 10), FQuat::Identity, ECC_Visibility, MyColSphere, TraceParam);

			for (auto HitRes : HitResults)
			{
				EPhysicalSurface mySurface = UGameplayStatics::GetSurfaceType(HitRes);
				//	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green,HitRes.GetActor()->GetName());

				if (HitRes.GetActor() && ProjectileSetting.Effect && HitRes.PhysMaterial.IsValid() && mySurface)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, TEXT("True for = ") + HitRes.GetActor()->GetName());
				//	UTypes::AddEffectBySurfaceType(HitRes.GetActor(), HitRes.BoneName, ProjectileSetting.Effect, mySurface);
				}
			}
			//	UTypes::AddEffectBySurfaceType(Hit.GetActor(), Hit.BoneName, ProjectileSetting.Effect, mySurfaceType);

		}


		UTypes::AddEffectBySurfaceType(Hit.GetActor(), Hit.BoneName, ProjectileSetting.Effect, mySurfaceType);

		if (Hit.BoneName.IsNone())
		{
			UGameplayStatics::ApplyDamage(OtherActor, ProjectileSetting.ProjectileDamage, GetInstigatorController(), this, NULL);
		}
		else
		{
			UGameplayStatics::ApplyPointDamage(OtherActor, ProjectileSetting.ProjectileDamage, RootComponent->GetForwardVector(), Hit, this->GetInstigatorController(), this, nullptr);
		}


		USkeletalMeshComponent* HittedMesh = Cast<USkeletalMeshComponent>(Hit.GetComponent());
		if (HittedMesh)
		{
			RenderTargetPrint(HitComp, OtherActor, OtherComponent, NormalImpulse, Hit, FindCollisionUVOnSkeletalMesh(Hit));
		}

		this->Destroy();
	}
}

USTRUCT(BlueprintType)
struct FSortVertexByDistance
{
	FSortVertexByDistance(const FVector& InSourceLocation) : SourceLocation(InSourceLocation)
	{

	}

	FVector SourceLocation;

	bool operator() (const FSoftSkinVertex A, const FSoftSkinVertex B) const
	{
		float DistanceA = FVector::DistSquared(SourceLocation, A.Position);
		float DistanceB = FVector::DistSquared(SourceLocation, B.Position);

		return DistanceA < DistanceB;
	}
};


FVector2D ADefaultProjectile::FindCollisionUVOnSkeletalMesh(const struct FHitResult& Hit)
{
	UPrimitiveComponent* HitPrimComp = Hit.Component.Get();
	USkeletalMeshComponent* SkelMeshComp = Cast<USkeletalMeshComponent>(HitPrimComp);
	USkeletalMesh* SkelMesh = SkelMeshComp->SkeletalMesh;
	FSkeletalMeshModel* SkelModel = SkelMesh->GetImportedModel();
	FSkeletalMeshLODModel* LODModel = &SkelModel->LODModels[0];

	//Coordinates to local
	const FVector LocalHitPos = HitPrimComp->GetComponentToWorld().InverseTransformPosition(Hit.Location);


	TArray<FSoftSkinVertex> Vertices;
	LODModel->GetVertices(Vertices);


	// Sort Shortest Vertex from LocalHitPos
	Vertices.Sort(FSortVertexByDistance(LocalHitPos));

	// Get three vertex for computing barycentric coords
	FVector Pos0 = Vertices[0].Position;
	FVector Pos1 = Vertices[3].Position;
	FVector Pos2 = Vertices[6].Position;

	FVector2D UV0 = Vertices[0].UVs[0];
	FVector2D UV1 = Vertices[3].UVs[0];
	FVector2D UV2 = Vertices[6].UVs[0];

	// Transform hit location from world to local space.
	// Find barycentric coords
	FVector BaryCoords = FMath::ComputeBaryCentric2D(LocalHitPos, Pos0, Pos1, Pos2);
	// Use to blend UVs
	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow, TEXT("BaryCoords.Z = ") + FVector(BaryCoords.Z).ToString() + TEXT("     UV2.Z = ") + UV2.ToString());
	}*/
	return (BaryCoords.X * UV0) + (BaryCoords.Y * UV1) + (BaryCoords.Z * UV2);
}



void ADefaultProjectile::InitProjectile(FProjectileInfo InitParam, FVector Direction)
{
	ProjectileSetting = InitParam;
	SpeedInit_Multicast(Direction, ProjectileSetting.ProjectileSpeed);
	

	
	this->SetLifeSpan(InitParam.ProjectileLifeTime);

}


void ADefaultProjectile::RenderTargetPrint_Implementation(class UPrimitiveComponent* HitComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse, const FHitResult& Hit, FVector2D HittedUV)
{
	//BP
}

void ADefaultProjectile::BulletCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void ADefaultProjectile::BulletCollisionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}


void ADefaultProjectile::SpeedInit_Multicast_Implementation(FVector Direction, float Speed)
{
	BulletProjectileMovement->Velocity = Direction * Speed;
	BulletProjectileMovement->Activate(true);
}

void ADefaultProjectile::SpawnHitDecal_Multicast_Implementation(UMaterialInterface* DecalMaterial, UPrimitiveComponent* OtherComponent, FHitResult Hit, bool bIsLikeBomb)
{
	UDecalComponent* myDecal;
	if (!bIsLikeBomb)
	{
		myDecal = UGameplayStatics::SpawnDecalAttached(DecalMaterial, FVector(BulletCollisionSphere->GetScaledSphereRadius()), OtherComponent,
			NAME_None, Hit.ImpactPoint, Hit.Normal.Rotation(), EAttachLocation::KeepWorldPosition, 50.0f);
		if (myDecal)
		{
			myDecal->SetFadeScreenSize(0.000f);
		}
	}
	else
	{
		myDecal = UGameplayStatics::SpawnDecalAttached(DecalMaterial, FVector(ProjectileSetting.BombRadius), OtherComponent,
			NAME_None, Hit.ImpactPoint, Hit.Normal.Rotation(), EAttachLocation::SnapToTargetIncludingScale, 10.0f);
		if (myDecal)
		{
			myDecal->SetFadeScreenSize(0.000f);
		}
	}
	
}

void ADefaultProjectile::SpawnHitFX_Multicast_Implementation(UParticleSystem* HitParticle, FHitResult Hit)
{

	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, FTransform(Hit.Normal.Rotation(),
			Hit.ImpactPoint, FVector(1.0f)));
	}
}

void ADefaultProjectile::SpawnHitSound_Multicast_Implementation(USoundBase* HitSound, FHitResult Hit)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, Hit.ImpactPoint);
}

