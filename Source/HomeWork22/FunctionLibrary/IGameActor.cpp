// Fill out your copyright notice in the Description page of Project Settings.


#include "IGameActor.h"

// Add default functionality here for any IIGameActor functions that are not pure virtual.


EPhysicalSurface IIGameActor::GetSurfaceType()
{
	return EPhysicalSurface::SurfaceType_Default;
}

TArray<UStateEffect*> IIGameActor::GetAllCurrentEffects()
{
	TArray<UStateEffect*> Effects;
	return Effects;
}

void IIGameActor::RemoveEffect(UStateEffect* RemoveEffect)
{

}

void IIGameActor::AddEffect(UStateEffect* NewEffect)
{

}


