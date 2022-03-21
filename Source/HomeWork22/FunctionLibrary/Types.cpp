// Fill out your copyright notice in the Description page of Project Settings.


#include "Types.h"
#include "HomeWork22.h"
#include "FunctionLibrary/IGameActor.h"

void UTypes::AddEffectBySurfaceType(AActor* HittedActor,FName ActorBoneName, TSubclassOf<UStateEffect> AddEffectClass, EPhysicalSurface SurfaceType)
{
	if (SurfaceType != EPhysicalSurface::SurfaceType_Default && HittedActor && AddEffectClass)
	{
	
		UStateEffect* myEffect = Cast<UStateEffect>(AddEffectClass->GetDefaultObject());
		if (myEffect)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, myEffect ? "myEffect = true" : "myInterface = false");

			for (int i = 0; i < myEffect->PossibleInteractSurface.Num(); i++)
			{
				if (myEffect->PossibleInteractSurface[i] == SurfaceType)
				{
				bool CanStackEffect = true;
				IIGameActor* myInterface = Cast<IIGameActor>(HittedActor);

				//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, myInterface ? "myInterface = true" : "myInterface = false");

				if (myInterface)
				{
					TArray<UStateEffect*> ArrayOfEffectsOnObject;
					ArrayOfEffectsOnObject = myInterface->GetAllCurrentEffects();

					//Проверка на стакабельность: Если нет - эффект обновляется, если да - добавляется ещё один
					if (!myEffect->CanBeStacked && ArrayOfEffectsOnObject.Num()>0)
					{
						//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("ArrayOfEffectsOnObject.Num = ") + FString::FromInt(ArrayOfEffectsOnObject.Num()));
						
						for (int j = 0; j < ArrayOfEffectsOnObject.Num(); j++)
						{
			
							if (ArrayOfEffectsOnObject[j]->GetClass() == AddEffectClass)
							{
							
								UStateEffect_ExecuteTimer* CurrentEffect = Cast<UStateEffect_ExecuteTimer>(ArrayOfEffectsOnObject[j]);

								if (CurrentEffect)
								{
									CurrentEffect->RefreshTimerDamageTick();
								}

								CanStackEffect = false;
								break;
							}
						}

					}
					else
					{
						CanStackEffect = true;
					}
				}
					if (CanStackEffect)
					{
						
						UStateEffect* NewEffect = NewObject<UStateEffect>(HittedActor, AddEffectClass);
						if (NewEffect)
						{
							//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("NewEffect Added"));
							NewEffect->InitObject(HittedActor, ActorBoneName);
						}
					}
				
				}
				
			}
			
		}
	}

}

