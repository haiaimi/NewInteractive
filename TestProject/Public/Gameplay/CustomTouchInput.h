// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CustomTouchInput.generated.h"

DECLARE_DELEGATE_TwoParams(FOnePointInputEvent, const FVector2D&, float);
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
};
