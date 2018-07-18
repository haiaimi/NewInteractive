// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/CustomTouchInterface.h"

/**
 *  触摸操作中使用的一些通用方法
 */
class TESTPROJECT_API TouchHelper
{
public:
	static void GetTouchInputInfo(class APlayerController* InPlayer, TArray<FVector>& OutTouchPoint);

	/** 测试触摸组件的触摸模式是否被包含在目标Actor中
	  * @Param TouchComponent     触摸组件
	  * @Param TargetActor        目标Actor
	  * @Param ComponentTouchType 当前触摸组件的触摸模式
	  * @Returns 如为true，则该目标Actor和触摸组件之间可以交互，false则不能
	  */
	static bool IsTouchTypeContained(class UObject* TouchComponent, class AActor* TargetActor, ECustomTouchType::Type ComponentTouchType);

	/** 获取指定截椎体内所有的Actor，包括相交的Actor,这里会使用FConvex来进行检测
	  * @Param InPlayer        请求调用的玩家
	  * @Param StartPoint      框选开始的点（屏幕）
	  * @Param EndPoint        框选结束得点（屏幕）
	  * @Out OutActors  返回与指定截椎体相交或被包含的所有Actor
	  */
	static void GetAllActorsInOrIntersectFrustrum(class APlayerController* InPlayer, FVector2D StartPoint, FVector2D EndPoint, TArray<AActor>& OutActors);
};
