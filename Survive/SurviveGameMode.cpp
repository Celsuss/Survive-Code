// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "SurviveGameMode.h"
#include "SurviveHUD.h"
#include "SurviveCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "SurvivePlayer.h"

ASurviveGameMode::ASurviveGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<ASurvivePlayer> PlayerPawnClassFinder(TEXT("/Game/BPs/BPSurvivePlayer"));		// Game/FirstPersonCPP/Blueprints/FirstPersonCharacter
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ASurviveHUD::StaticClass();

	
}
