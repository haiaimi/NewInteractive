// Fill out your copyright notice in the Description page of Project Settings.

#include "TestProjectGameModeBase.h"
#include "Public/MainController.h"
#include "UI/TestProjectHUD.h"

ATestProjectGameModeBase::ATestProjectGameModeBase(const FObjectInitializer& Initializer)
{
	PlayerControllerClass = AMainController::StaticClass();
	HUDClass = ATestProjectHUD::StaticClass();
}


