// Fill out your copyright notice in the Description page of Project Settings.


#include "LFP_Create_GameMode.h"

ALFP_Create_GameMode::ALFP_Create_GameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ALFP_Create_GameMode::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/LFP/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/LFP/Blueprints/BP_CreateMode_PlayerController"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}
