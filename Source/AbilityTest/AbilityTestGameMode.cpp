// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilityTestGameMode.h"
#include "AbilityTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAbilityTestGameMode::AAbilityTestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
