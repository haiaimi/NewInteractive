// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOrigin, Log, All);

#define COLLISION_LOCKERRAY     ECollisionChannel::ECC_GameTraceChannel1
#define COLLISION_INVENTORYRAY  ECollisionChannel::ECC_GameTraceChannel2

class FWarSimulateModule :public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule()override;
};