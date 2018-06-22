// Fill out your copyright notice in the Description page of Project Settings.

#include "TestProjectGameModeBase.h"
#include "UI/TestProjectHUD.h"
#include "Gameplay/MainController.h"
#include "Gameplay/GroundSpectatorPawn.h"

ATestProjectGameModeBase::ATestProjectGameModeBase(const FObjectInitializer& Initializer)
{
	PlayerControllerClass = AMainController::StaticClass();
	HUDClass = ATestProjectHUD::StaticClass();
	DefaultPawnClass = AGroundSpectatorPawn::StaticClass();
	SpectatorClass = AGroundSpectatorPawn::StaticClass();
}


