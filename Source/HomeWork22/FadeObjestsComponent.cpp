// Fill out your copyright notice in the Description page of Project Settings.


#include "FadeObjestsComponent.h"
#include "Character/HomeWork22Character.h"

// Sets default values for this component's properties
UFadeObjestsComponent::UFadeObjestsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFadeObjestsComponent::BeginPlay()
{
	Super::BeginPlay();
	if (FadeMaterial && FadeEnabled)
	{
		FTimerHandle myTimerHandle;
		myParams.bReturnPhysicalMaterial = true;

		//temp array for actors on the scene 
		TArray<AActor*> TempArray;

		//array for all actors that should be ignored
		TArray<AActor*> ArrayOfActorsToIgnore;


		if (ToIgnore.Num() != 0)
		{
			for (auto& TempElem : ToIgnore)
			{
				UGameplayStatics::GetAllActorsOfClass(this, TempElem, TempArray);
				ArrayOfActorsToIgnore.Append(TempArray);
			}

			myParams.AddIgnoredActors(ArrayOfActorsToIgnore);
		}
		GetWorld()->GetTimerManager().SetTimer(myTimerHandle, this, &UFadeObjestsComponent::AddObjectsToFade, fadeSpeed, true);

	}
	
}




void UFadeObjestsComponent::AddObjectsToFade()
{
	AHomeWork22Character* BP_Character = Cast<AHomeWork22Character>(GetOwner());
	if (BP_Character->bIsAlive)
	{
		if (GEngine->GetFirstLocalPlayerController(GetWorld()))
		{

		
		FVector CameraLocation = GEngine->GetFirstLocalPlayerController(GetWorld())->PlayerCameraManager->GetCameraLocation();
		FVector CharacterLocation = GEngine->GetFirstLocalPlayerController(GetWorld())->GetPawn()->GetActorLocation();


		// create a collision 
		FCollisionShape MyColCapsule = FCollisionShape::MakeCapsule(CapsuleRadius, (CameraLocation - CharacterLocation).Size() / 2);

		FQuat QuaternionForCapsule = FQuat(FRotator((UKismetMathLibrary::FindLookAtRotation(CharacterLocation, CameraLocation).Pitch) - (90), UKismetMathLibrary::FindLookAtRotation(CharacterLocation, CameraLocation).Yaw, 0));
		
		//Отрисовка капсулы захвата

		if (DebugCapsule)
		{
			DrawDebugCapsule(GetWorld(), (CameraLocation + CharacterLocation) / 2, MyColCapsule.GetCapsuleHalfHeight(), MyColCapsule.GetCapsuleRadius(), QuaternionForCapsule, FColor::Red, false, 1);
			DrawDebugCapsule(GetWorld(), (CameraLocation + CharacterLocation) / 2 + 1, MyColCapsule.GetCapsuleHalfHeight(), MyColCapsule.GetCapsuleRadius(), QuaternionForCapsule, FColor::Green, false, 1);
		}


		bool isHit = GetWorld()->SweepMultiByChannel(OutHits, (CameraLocation + CharacterLocation) / 2,
			(CameraLocation + CharacterLocation) / 2 + 1, QuaternionForCapsule, WorkChannel, MyColCapsule, myParams);

		//Середина капсулы
		//DrawDebugPoint(GetWorld(), (CameraLocation + CharacterLocation) / 2, 30.0f, FColor::Green, false, 1);
		//Rotation between camera and character



		if (isHit)
		{
			//Первый тик
				//CapsulePrimitiveArray.Empty();
			if (CapsulePrimitiveArray.Num() == 0 && PrimitiveArrayToCompare.Num() == 0)
			{
				for (FHitResult& Hit : OutHits)
				{
					CapsulePrimitiveArray.Add(Hit.GetComponent());
					PrimitiveArrayToCompare.Add(Hit.GetComponent());
					MaterialArrayToCompare.Add(Hit.GetComponent()->GetMaterial(0));
				}

				CapsulePrimitiveArray.Empty();

				//	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, TEXT("ONE TIME"));
			}
			else
			{
				// 
				for (FHitResult& Hit : OutHits)
				{
					if (!CapsulePrimitiveArray.Contains(Hit.GetComponent()))
					{
						CapsulePrimitiveArray.Add(Hit.GetComponent());
					}
				}

				if (PrimitiveArrayToCompare.Num() == 0)
				{
					for (FHitResult& Hit : OutHits)
					{
						PrimitiveArrayToCompare.Add(Hit.GetComponent());
						MaterialArrayToCompare.Add(Hit.GetComponent()->GetMaterial(0));
					}
				}
				//Находим разные элементы
				MakeFadeOrNot(false);

				//	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Purple, FString("Primitive array num: ") + FString::FromInt(PrimitiveArrayToCompare.Num()));
				//	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString("Capsule primitive num: ") + FString::FromInt(CapsulePrimitiveArray.Num()));
			}
		}
		else
		{
			//Вызываем функцию появления всех материалов
			CapsulePrimitiveArray.Empty();

			MakeFadeOrNot(true);
		}
		CapsulePrimitiveArray.Empty();
		}
	}


}

void UFadeObjestsComponent::MakeFadeOrNot(bool AllAppeares)
{
	PrimitiveArrayToCompare.Append(PrimitiveArrayTemp);
	MaterialArrayToCompare.Append(MaterialArrayTemp);

	PrimitiveArrayTemp.Empty();
	MaterialArrayTemp.Empty();

	int countOfDif = 0;

	if (AllAppeares)
	{
		for (int i = 0; i < PrimitiveArrayToCompare.Num(); i++)
		{
			PrimitiveArrayToCompare[i]->SetMaterial(0, MaterialArrayToCompare[i]);
		}
		PrimitiveArrayToCompare.Empty();
		MaterialArrayToCompare.Empty();
	}
	else
	{
		for (int i = 0; i < PrimitiveArrayToCompare.Num(); i++)
		{
			if (CapsulePrimitiveArray.Contains(PrimitiveArrayToCompare[i]))
			{
				PrimitiveArrayTemp.Add(PrimitiveArrayToCompare[i]);
				MaterialArrayTemp.Add(MaterialArrayToCompare[i]);
				PrimitiveArrayToCompare[i]->SetMaterial(0, FadeMaterial);
			}
			else if (!CapsulePrimitiveArray.Contains(PrimitiveArrayToCompare[i]))
			{
				PrimitiveArrayToCompare[i]->SetMaterial(0, MaterialArrayToCompare[i]);
			}
		}

		PrimitiveArrayToCompare.Empty();
		MaterialArrayToCompare.Empty();
	}
}

// Called every frame
void UFadeObjestsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

