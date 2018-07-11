// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CustomTouchInput.generated.h"

DECLARE_DELEGATE_TwoParams(FOnePointInputEvent, const FVector2D&, float);
DECLARE_DELEGATE_ThreeParams(FTwoPointInputEvent, const FVector2D&, const FVector2D&, float);
DECLARE_DELEGATE_TwoParams(FTwoMorePointInputEvent, const TArray<FVector2D>&, float);  //两指以上的操作

// 下面是两个绑定触摸事件的宏，有单指和两指的
#define  BIND_1P_ACTION(Handler, ActionKey, ActionState, UserObject, Delegate)   \
{ \
	int32 Idx = Handler->ActionBindings1P.AddZeroed(); \
	Handler->ActionBindings1P[Idx].Key = ActionKey; \
	Handler->ActionBindings1P[Idx].KeyState = ActionState; \
	Handler->ActionBindings1P[Idx].ActionDelegate.BindUObject(UserObject, Delegate); \
}

#define BIND_2P_ACTION(Handler, ActionKey, ActionState, UserObject, Delegate)  \
{ \
	int32 Idx = Handler->ActionBindings2P.AddZeroed(); \
	Handler->ActionBindings2P[Idx].Key = ActionKey; \
	Handler->ActionBindings2P[Idx].KeyState = ActionState; \
	Handler->ActionBindings2P[Idx].ActionDelegate.BindUObject(UserObject, Delegate); \
}

#define BIND_2MP_ACTION(Handler, ActionKey, ActionState, UserObject, Delegate) \
{ \
	int32 Idx = Handler->ActionBindings2MP.AddZeroed(); \
	Handler->ActionBindings2MP[Idx].Key = ActionKey; \
	Handler->ActionBindings2MP[Idx].KeyState = ActionState; \
	Handler->ActionBindings2MP[Idx].ActionDelegate.BindUObject(UserObject, Delegate); \
}

/**触摸输入的类型*/
namespace EGameTouchKey
{
	enum Type
	{
		Tap,       //单击
		DoubleTap, //双击
		Hold,	   //长按
		Swipe,     //拖拽
		Pinch,     //双指缩放
		FivePoints,    //五指触控，一般用于界面收缩
	};
}

/**下面的顺序同时还是触摸响应的优先级*/
namespace ETouchEventLevel
{
	enum Type
	{
		OnePoint,
		TwoPoints,
		ThreePoints,
		FourPoints,
		FivePoints,
	};
}

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

/**两指触控对应的信息*/
struct FActionBinding2P
{
	/**输入触发类型*/
	EGameTouchKey::Type Key;

	/**当前触发状态，有Pressed,Released,Repeat*/
	TEnumAsByte<EInputEvent> KeyState;

	/**事件代理*/
	FTwoPointInputEvent ActionDelegate;
};

/**两指以上触控对应信息*/
struct FActionBinding2MP
{
	EGameTouchKey::Type Key;

	TEnumAsByte<EInputEvent> KeyState;

	FTwoMorePointInputEvent ActionDelegate;
};

/**存储某个触摸状态的具体数据*/
struct FSimpleKeyState
{
	/**该输入类型的3个状态*/
	uint8 Events[3];

	/**当前组合是否被按下*/
	uint8 bDown : 1;

	FVector2D Position1;

	FVector2D Position2;

	/**多余两个触摸点的数组*/
	TArray<FVector2D> TwoMorePositions;

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

	FVector2D*const GetTouchAnchors();
	
private:
	/**更新游戏触摸输入状态*/
	void UpdateGameKeys(float DeltaTime);

	void ProcessKeyStates(float DeltaTime);

	/**检测单触摸点按下状态*/
	void DetectOnePointActions(bool bCurrentState, bool bPrevState, float DeltaTime, const FVector2D& CurrentPosition, float& DownTime);
	void DetectTwoPointsActions(bool bCurrentState, bool bPrevState, float DeltaTime, const FVector2D& CurrentPosition1, const FVector2D& CurrentPosition2);
	void DetectFivePointsActions(bool bCurrentState, bool bPrevState, float DeltaTime, const TArray<FVector2D>& CurrentPositions);

public:
	// 用于单指按下三个状态的代理，Pressed，Released，Repeat
	FOnePointInputEvent OnePointEvent[3];     

	TArray<FActionBinding1P> ActionBindings1P;
	TArray<FActionBinding2P> ActionBindings2P;
	TArray<FActionBinding2MP> ActionBindings2MP;

private:
	/**用于存储当前屏幕上触摸组合状态*/
	TMap<EGameTouchKey::Type, FSimpleKeyState> KeyStateMap;

	/**上一次更新是否点击了屏幕*/
	uint32 PreTouchedState;

	/**当前按键状态*/
	EInputEvent CurKeyEvent; 

	/**当前触摸的最高优先级*/
	ETouchEventLevel::Type CurTouchLevel;

	FVector2D TouchAnchors[2];

	float Touch0DownTime;

	float TwoPointDownTime;

	/**五指触摸按下的时间*/
	float FivePointDownTime;

	uint8 bTwoPointTouched : 1;

	float DownTime;    //点击时间

	float TouchInterval;  //单击屏幕事件间隔
};
