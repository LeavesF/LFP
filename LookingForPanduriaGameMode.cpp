// Copyright Epic Games, Inc. All Rights Reserved.

#include "LookingForPanduriaGameMode.h"
#include "LookingForPanduriaPlayerController.h"
#include "LookingForPanduriaCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALookingForPanduriaGameMode::ALookingForPanduriaGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ALookingForPanduriaPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/LFP/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/LFP/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}