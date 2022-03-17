// Fill out your copyright notice in the Description page of Project Settings.


#include "DropItemComponent.h"
#include "PickupActor.h"
#include "UHomeWork22GameInstance.h"

// Sets default values for this component's properties
UDropItemComponent::UDropItemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDropItemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDropItemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UDropItemComponent::DropRandomItem(FVector DropDirection, int32 CommonChance, int32 RareChance, int32 EpicChance, int32 LegendaryChance)
{
	AActor* ComponentOwner = Cast<AActor>(GetOwner());
	if (ComponentOwner)
	{
		FVector VectorToDrop;
		if (DropDirection != FVector(0))
		{
			DropDirection.Normalize();
			VectorToDrop = DropDirection * 400;
		}
		else
		{
			VectorToDrop = ComponentOwner->GetRootComponent()->GetForwardVector() * 400;
		}

		FTransform NewTransform;
		NewTransform.SetLocation(ComponentOwner->GetRootComponent()->GetComponentLocation());
		NewTransform.SetRotation(ComponentOwner->GetRootComponent()->GetComponentRotation().Quaternion());
		FActorSpawnParameters SpawnParams;

		UHomeWork22GameInstance* myGI = Cast<UHomeWork22GameInstance>(GetWorld()->GetGameInstance());
		if (myGI)
		{

			if ((CommonChance + RareChance + EpicChance + LegendaryChance) <= 0 || (CommonChance + RareChance + EpicChance + LegendaryChance) > 100)
			{
				CommonChance = 60;
				RareChance = 20;
				EpicChance = 15;
				LegendaryChance = 5;
			}
			
				APickupActor* PickUpActor = Cast<APickupActor>(GetWorld()->SpawnActor(myGI->GetRandomActorToDrop(CommonChance, RareChance, EpicChance, LegendaryChance), &NewTransform, SpawnParams));
				if (PickUpActor)
				{
					if (!PickUpActor->CollisionSphere->IsSimulatingPhysics())
					{
						PickUpActor->CollisionSphere->SetSimulatePhysics(true);
					}
					PickUpActor->CollisionSphere->AddImpulse(VectorToDrop, NAME_None, true);

					return true;

					/*if (GEngine)
					{
						GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Red, PickUpActor->GetName());
					}*/
				}
			
		}
	}
	return false;
}

void UDropItemComponent::DropItem(FVector DropDirection)
{
	
}

