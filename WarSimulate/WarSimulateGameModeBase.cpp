﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "WarSimulateGameModeBase.h"
#include "UI/WarSimulateHUD.h"
#include "Gameplay/PlatformController.h"
#include "Gameplay/GroundSpectatorPawn.h"

AWarSimulateGameModeBase::AWarSimulateGameModeBase(const FObjectInitializer& Initializer)
{
	PlayerControllerClass = APlatformController::StaticClass();
	HUDClass = AWarSimulateHUD::StaticClass();
	DefaultPawnClass = AGroundSpectatorPawn::StaticClass();
	SpectatorClass = AGroundSpectatorPawn::StaticClass();
}


