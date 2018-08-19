// Fill out your copyright notice in the Description page of Project Settings.

#include "WarSimulateProject.h"
#include "Modules/ModuleManager.h"
#include "OriginHelper.h"


void FWarSimulateModule::StartupModule()
{
	OriginHelper::Init(FString(TEXT("ConfigRes/FlightPlatform")));
}

IMPLEMENT_PRIMARY_GAME_MODULE( FWarSimulateModule, WarSimulate, "WarSimulate" );

DEFINE_LOG_CATEGORY(LogOrigin)