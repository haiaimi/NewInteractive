// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/CustomTouchInterface.h"

/**
 * 
 */
class TESTPROJECT_API TouchHelper
{
public:
	static void GetTouchInputInfo(class APlayerController* InPlayer, TArray<FVector>& OutTouchPoint);

	/** 测试触摸组件的触摸模式是否被包含在目标Actor中
	  * @Param TouchComponent     触摸组件
	  * @Param TargetActor        目标Actor
	  * @Param ComponentTouchType 当前触摸组件的触摸模式
	  */
	static bool IsTouchTypeContained(class UObject* TouchComponent, class AActor* TargetActor, TEnumAsByte<ECustomTouchType::Type> ComponentTouchType);
};
