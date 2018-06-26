// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTest, Log, All);

#define COLLISION_LOCKERRAY     ECollisionChannel::ECC_GameTraceChannel1
#define COLLISION_INVENTORYRAY  ECollisionChannel::ECC_GameTraceChannel2

/**触摸输入的类型*/
namespace EGameTouchKey
{
	enum Type
	{
		Tap,      //单击
		Hold,	  //长按
		Swipe,    //拖拽
		Pinch,    //双指缩放
	};
}