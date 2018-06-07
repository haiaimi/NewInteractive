// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainController.h"

/**
 * 该项目通用工具类，主要是静态成员，如屏幕输出和日志
 */

class TESTPROJECT_API TestProjectHelper
{
public:
	static void Debug_ScreenMessage(FString&& InString, FColor FontColor = FColor::Blue);

	static void Debug_LogMessage(FString&& InString);

	/**根据屏幕空间计算其所在的世界空间*/
	static void DeProjectScreenToWorld(const AMainController* PlayerContorl, FVector& OutWorldPos, FVector& OutWorldDir);
};
