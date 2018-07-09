// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class TESTPROJECT_API TouchHelper
{
public:
	static void GetTouchInputInfo(class APlayerController* InPlayer, TArray<FVector>& OutTouchPoint);
};
