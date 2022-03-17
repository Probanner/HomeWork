// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "HomeWork22GameMode.h"
#include "HomeWork22PlayerController.h"
#include "Character/HomeWork22Character.h"
#include "UObject/ConstructorHelpers.h"

AHomeWork22GameMode::AHomeWork22GameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AHomeWork22PlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	/*
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/Character/BP_Character"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	*/
	DefaultPawnClass = AHomeWork22Character::StaticClass();

}
