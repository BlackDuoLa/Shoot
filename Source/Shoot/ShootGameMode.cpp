// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShootGameMode.h"
#include "ShootCharacter.h"
#include "UObject/ConstructorHelpers.h"

AShootGameMode::AShootGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
