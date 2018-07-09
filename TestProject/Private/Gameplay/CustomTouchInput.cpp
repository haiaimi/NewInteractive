// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomTouchInput.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

static const float DoubleTapMaxDistance = 10.f;    //双击事件情况下，两次触摸的最大距离（一般双击状态，两次点击距离过大则不算）
static const float DoubleTapIntervalTime = 0.25f;   //双击时间间隔

UCustomTouchInput::UCustomTouchInput(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	, PreTouchedState(0)
	, Touch0DownTime(0.f)
	, TwoPointDownTime(0.f)
	, bTwoPointTouched(false)
	, TouchInterval(0.f)
{

}

void UCustomTouchInput::UpdateInputStates(float DeltaTime)
{
	UpdateGameKeys(DeltaTime);
	ProcessKeyStates(DeltaTime);
}

FVector2D*const UCustomTouchInput::GetTouchAnchors()
{
	return TouchAnchors;
}

void UCustomTouchInput::UpdateGameKeys(float DeltaTime)
{
	if (!GetOuter())return;
	APlayerController* Controller = Cast<APlayerController>(GetOuter());
	
	uint32 CurrentTouchState = 0;
	for (int32 i = 0; i < ARRAY_COUNT(Controller->PlayerInput->Touches); i++)
	{
		if (Controller->PlayerInput->Touches[i].Z != 0)
		{
			CurrentTouchState |= (1 << i);
		}
	}

	FVector2D LocalPosition1 = FVector2D(Controller->PlayerInput->Touches[0]);
	FVector2D LocalPosition2 = FVector2D(Controller->PlayerInput->Touches[1]);

	DetectOnePointActions(CurrentTouchState & 1, PreTouchedState & 1, DeltaTime, LocalPosition1, Touch0DownTime);
	DetectTwoPointActions((CurrentTouchState & 1) && (CurrentTouchState & 2), (PreTouchedState & 1) && (PreTouchedState & 2), DeltaTime, LocalPosition1, LocalPosition2);

	//if (OwnerController && OwnerController->PlayerInput->Touches[0].Z != 0)
	//{
	//	CurrentTouchState |= (1 << 0);       //此时有一个触摸点
	//}

	//FVector Temp = OwnerController->PlayerInput->Touches[0];
	//FVector2D TouchPoint(Temp);

	//if (CurrentTouchState)
	//{
	//	if (!PreTouched)
	//	{
	//		DownTime = 0.f;
	//		OnePointEvent[IE_Pressed].ExecuteIfBound(TouchPoint, DownTime);
	//	}

	//	if (PreTouched)
	//	{
	//		DownTime += DeltaTime;
	//		OnePointEvent[IE_Repeat].ExecuteIfBound(TouchPoint, DownTime);
	//	}
	//}
	//else
	//{
	//	if (PreTouched)
	//	{
	//		OnePointEvent[IE_Released].ExecuteIfBound(TouchPoint, DownTime);
	//		DownTime = 0.f;
	//	}
	//}
	PreTouchedState = CurrentTouchState;
}

void UCustomTouchInput::DetectOnePointActions(bool bCurrentState, bool bPrevState, float DeltaTime, const FVector2D& CurrentPosition, float& DownTime)
{
	if (bCurrentState)
	{
		if (!bPrevState)
		{
			DownTime = 0;
		}
		//else
		//{
		//	FSimpleKeyState& DoubleTap = KeyStateMap.FindOrAdd(EGameTouchKey::DoubleTap);
		//	const FVector2D TwoPointsDistance = DoubleTap.Position1 - CurrentPosition;

		//	if (TwoPointsDistance.Size() < DoubleTapMaxDistance && (DownTime - DoubleTap.DownTime) < DoubleTapIntervalTime)   //判断是否符合双击状态
		//	{
		//		DoubleTap.Events[IE_Pressed]++;
		//	}
		//}

		FSimpleKeyState& SwipeState = KeyStateMap.FindOrAdd(EGameTouchKey::Swipe);
		if (SwipeState.bDown)
		{
			SwipeState.Events[IE_Repeat]++;       //Swipe开始
			SwipeState.Position1 = CurrentPosition;
			SwipeState.DownTime = DownTime;
		}
		else if (!bPrevState)
		{
			SwipeState.Events[IE_Pressed]++;     //Swipe更新
			SwipeState.Position1 = CurrentPosition;
			SwipeState.DownTime = DownTime;
		}

		FSimpleKeyState& Tap = KeyStateMap.FindOrAdd(EGameTouchKey::Tap);   //单击事件
		if (!bPrevState)
		{
			Tap.Events[IE_Pressed]++; 
			Tap.Position1 = CurrentPosition;
			Tap.DownTime = DownTime;
		}

		//FSimpleKeyState& DoubleTap = KeyStateMap.FindOrAdd(EGameTouchKey::DoubleTap);
		//if (!DoubleTap.bDown)
		//{
		//	DoubleTap.DownTime = DownTime;
		//	DoubleTap.Position1 = CurrentPosition;
		//}
		//else
		//{
		//	DoubleTap.Events[IE_Released]++;
		//}

		//if (bPrevState)      //
		//{
		//	const FVector2D TwoPointsDistance = DoubleTap.Position1 - CurrentPosition;

		//	if (TwoPointsDistance.Size() < DoubleTapMaxDistance && (DownTime - DoubleTap.DownTime) < DoubleTapIntervalTime)   //判断是否符合双击状态
		//	{
		//		DoubleTap.Events[IE_Pressed]++;    //此时触发双击触摸事件
		//	}
		//}

		DownTime += DeltaTime;	
	}
	else
	{
		if (bPrevState)
		{
			FSimpleKeyState& SwipeState = KeyStateMap.FindOrAdd(EGameTouchKey::Swipe);
			if (SwipeState.bDown)
			{
				SwipeState.Events[IE_Released]++;    //此时Swipe已经结束
				SwipeState.Position1 = CurrentPosition;
				SwipeState.DownTime = DownTime;
			}

			FSimpleKeyState& Tap = KeyStateMap.FindOrAdd(EGameTouchKey::Tap);   //单击事件
			if (Tap.bDown)
			{
				if (DownTime - Tap.DownTime < DoubleTapIntervalTime && GetWorld())  //单击持续时间应该要限制在一定时间内
				{
					if (TouchInterval != 0.f && GetWorld()->TimeSeconds - TouchInterval < DoubleTapIntervalTime && (Tap.Position1 - CurrentPosition).Size() < DoubleTapMaxDistance)
					{
						FSimpleKeyState& DoubleTap = KeyStateMap.FindOrAdd(EGameTouchKey::DoubleTap);
						DoubleTap.Events[IE_Pressed]++;    //触发双击事件
						TouchInterval = 0.f;   //间隔事件归零
					}
					else
						TouchInterval = GetWorld()->TimeSeconds;

					Tap.Events[IE_Released]++;
					Tap.Position1 = CurrentPosition;
					Tap.DownTime = DownTime;
				}
			}
		}
	}
}

void UCustomTouchInput::DetectTwoPointActions(bool bCurrentState, bool bPrevState, float DeltaTime, const FVector2D& CurrentPosition1, const FVector2D& CurrentPosition2)
{
	bTwoPointTouched = bCurrentState;
	if (bCurrentState)
	{
		if (!bPrevState)
		{
			TwoPointDownTime = 0.f;
			TouchAnchors[0] = CurrentPosition1;
			TouchAnchors[1] = CurrentPosition2;
			FSimpleKeyState& PinchState = KeyStateMap.FindOrAdd(EGameTouchKey::Pinch);
			PinchState.Events[IE_Pressed]++;
			PinchState.Position1 = CurrentPosition1;
			PinchState.Position2 = CurrentPosition2;
			PinchState.DownTime = TwoPointDownTime;
		}

		FSimpleKeyState& PinchState = KeyStateMap.FindOrAdd(EGameTouchKey::Pinch);
		if (PinchState.bDown)
		{
			PinchState.Events[IE_Repeat]++;    //如果该触摸状态按下就开始进入Update状态
			PinchState.Position1 = CurrentPosition1;
			PinchState.Position2 = CurrentPosition2;
			PinchState.DownTime = TwoPointDownTime;
		}

		TwoPointDownTime += DeltaTime;
	}
	else
	{
		if (bPrevState)
		{
			FSimpleKeyState& PinchState = KeyStateMap.FindOrAdd(EGameTouchKey::Pinch);
			if (PinchState.bDown)
			{
				PinchState.Events[IE_Released]++;
				PinchState.Position1 = CurrentPosition1;
				PinchState.Position2 = CurrentPosition2;
				PinchState.DownTime = TwoPointDownTime;
			}
		}
	}
}

void UCustomTouchInput::ProcessKeyStates(float DeltaTime)
{
	//遍历所有绑定事件，查看是否需要触发
	for (const auto& AB : ActionBindings1P)
	{
		const FSimpleKeyState* KeyState = KeyStateMap.Find(AB.Key);

		if (KeyState && KeyState->Events[AB.KeyState] > 0)
		{
			AB.ActionDelegate.ExecuteIfBound(KeyState->Position1, KeyState->DownTime);
		}
	}

	for (const auto& AB : ActionBindings2P)
	{
		const FSimpleKeyState* KeyState = KeyStateMap.Find(AB.Key);

		if (KeyState && KeyState->Events[AB.KeyState] > 0)
		{
			AB.ActionDelegate.ExecuteIfBound(KeyState->Position1, KeyState->Position2, KeyState->DownTime);
		}
	}

	for (TMap<EGameTouchKey::Type, FSimpleKeyState>::TIterator It(KeyStateMap); It; ++It)
	{
		FSimpleKeyState* const KeyState = &It.Value();

		if (KeyState->Events[IE_Pressed])
		{
			KeyState->bDown = true;
		}
		else if (KeyState->Events[IE_Released])
		{
			KeyState->bDown = false;
		}

		//清零KeyState中的所有状态
		FMemory::Memzero(KeyState->Events, sizeof(KeyState->Events));
	}
}