// Fill out your copyright notice in the Description page of Project Settings.

#include "TestProjectGameModeBase.h"
#include "Public/MainController.h"

ATestProjectGameModeBase::ATestProjectGameModeBase(const FObjectInitializer& Initializer)
{
	PlayerControllerClass = AMainController::StaticClass();
}


