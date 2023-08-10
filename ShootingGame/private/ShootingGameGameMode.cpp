// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShootingGameGameMode.h"
#include "ShootingGameCharacter.h"

#include "UObject/ConstructorHelpers.h"

#include "WCHub.h"

#include "Blueprint/UserWidget.h"

#include "GameModes/GSTeamDeathMatch.h"

AShootingGameGameMode::AShootingGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	// set game state base class
	GameStateClass = AGSTeamDeathMatch::StaticClass();
}
