// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TestProject.h"
#include "CustomTouchInput.generated.h"

DECLARE_DELEGATE_TwoParams(FOnePointInputEvent, const FVector2D&, float);
DECLARE_DELEGATE_ThreeParams(FTwoPointInputEvent, const FVector2D&, const FVector2D&, float);

/**单指触控输入对应的信息*/
struct FActionBinding1P
{
	/**输入触发类型*/
	EGameTouchKey::Type Key;      

	/**当前触发状态，有Pressed,Released,Repeat*/
	TEnumAsByte<EInputEvent> KeyState;

	/**事件代理*/
	FOnePointInputEvent ActionDelegate;
};

/**多指触控对应的信息*/
struct FActionBinding2P
{
	/**输入触发类型*/
	EGameTouchKey::Type Key;

	/**当前触发状态，有Pressed,Released,Repeat*/
	TEnumAsByte<EInputEvent> KeyState;

	/**事件代理*/
	FTwoPointInputEvent ActionDelegate;
};

struct FSimpleKeyState
{
	/**该输入类型的3个状态*/
	uint8 Events[3];

	FVector2D Position1;

	FVector2D Position2;

	float DownTime;

	FSimpleKeyState()
	{
		FMemory::Memzero(this, sizeof(FSimpleKeyState));
	}
};

/**
 * 多点触控输入控制类
 */
UCLASS()
class TESTPROJECT_API UCustomTouchInput : public UObject
{
	GENERATED_BODY()
	
public:
	UCustomTouchInput(const FObjectInitializer& ObjectInitializer);

	/**更新输入状态*/
	void UpdateInputStates(float DeltaTime);
	
private:
	/**更新游戏触摸输入状态*/
	void UpdateGameKeys(float DeltaTime);

	void ProcessKeyStates(float DeltaTime);

public:
	// 用于单指按下三个状态的代理，Pressed，Released，Repeat
	FOnePointInputEvent OnePointEvent[3];     

private:
	class AMainController * OwnerController;

	/**上一次更新是否点击了屏幕*/
	bool PreTouched;

	/**当前按键状态*/
	EInputEvent CurKeyEvent; 

	float DownTime;    //点击时间
};
