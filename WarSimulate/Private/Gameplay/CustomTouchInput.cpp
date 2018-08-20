// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomTouchInput.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "OriginHelper.h"

static const float DoubleTapMaxDistance = 10.f;    //双击事件情况下，两次触摸的最大距离（一般双击状态，两次点击距离过大则不算）
static const float DoubleTapIntervalTime = 0.25f;   //双击时间间隔
static const float CheckTouchLevelTime = 0.1f;     //检测触摸事件类型的等级
static const float HoldTime = 0.8f;                 //按下持续时间

UCustomTouchInput::UCustomTouchInput(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	, PreTouchedState(0)
	, CurTouchLevel(ETouchEventLevel::OnePoint)
	, Touch0DownTime(0.f)
	, TwoPointDownTime(0.f)
	, FivePointDownTime(0.f)
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
	static uint32 PreTouchNum = 0;

	if (!GetOuter())return;
	APlayerController* Controller = Cast<APlayerController>(GetOuter());
	
	uint32 CurrentTouchState = 0;
	uint32 CurrentTouchNum = 0;
	TArray<FVector2D> TouchPoints;

	for (int32 i = 0; i < ARRAY_COUNT(Controller->PlayerInput->Touches); i++)
	{
		if (Controller->PlayerInput->Touches[i].Z != 0)
		{
			CurrentTouchState |= (1 << i);
			CurrentTouchNum++;
			TouchPoints.Add(FVector2D(Controller->PlayerInput->Touches[i]));
		}
	}

	FVector2D LocalPosition1 = FVector2D(Controller->PlayerInput->Touches[0]);
	FVector2D LocalPosition2 = FVector2D(Controller->PlayerInput->Touches[1]);
	//CurTouchLevel = (ETouchEventLevel::Type)(CurrentTouchNum - 1);
	//TestProjectHelper::Debug_ScreenMessage(LocalPosition1.ToString());

	DetectFivePointsActions(CurrentTouchNum == 5, PreTouchNum == 5, DeltaTime, TouchPoints);
	DetectTwoPointsActions((CurrentTouchState & 1) && (CurrentTouchState & 2), (PreTouchedState & 1) && (PreTouchedState & 2), DeltaTime, LocalPosition1, LocalPosition2);
	DetectOnePointActions(CurrentTouchState & 1, PreTouchedState & 1, DeltaTime, LocalPosition1, Touch0DownTime);

	PreTouchedState = CurrentTouchState;
	PreTouchNum = CurrentTouchNum;
}

void UCustomTouchInput::DetectOnePointActions(bool bCurrentState, bool bPrevState, float DeltaTime, const FVector2D& CurrentPosition, float& DownTime)
{
	if (bCurrentState)
	{
		if (!bPrevState)
		{
			DownTime = 0;
		}

		FSimpleKeyState& HoldState = KeyStateMap.FindOrAdd(EGameTouchKey::Hold);
		if (!bPrevState)
		{
			HoldState.DownTime = DownTime;
			HoldState.Position1 = CurrentPosition;
		}

		FSimpleKeyState& Tap = KeyStateMap.FindOrAdd(EGameTouchKey::Tap);   //单击事件
		if (!bPrevState)
		{
			Tap.Events[IE_Pressed]++; 
			Tap.Position1 = CurrentPosition;
			Tap.DownTime = DownTime;
		}

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

		DownTime += DeltaTime;

		//在一定时间内进行检测是否有其他按键按下，延迟检测，可能会损失一些精确度
		if (DownTime <= CheckTouchLevelTime)
			return;

		if (DownTime > CheckTouchLevelTime)
			if (CurTouchLevel > ETouchEventLevel::OnePoint)
				return;

		if (!HoldState.bDown)
		{
			if ((CurrentPosition - HoldState.Position1).Size() < 20.f)
			{
				if (DownTime - HoldState.DownTime >= HoldTime)
				{
					HoldState.Events[IE_Pressed]++;
					HoldState.Position1 = CurrentPosition;
					HoldState.DownTime = DownTime;
				}
			}
			else
			{
				if (DownTime - HoldState.DownTime <= HoldTime)
				{
					HoldState.Position1 = CurrentPosition;
					HoldState.DownTime = DownTime;
					HoldState.bDown = DownTime;
				}
			}
		}
	}
	else
	{
		if (bPrevState)
		{
			FSimpleKeyState& HoldState = KeyStateMap.FindOrAdd(EGameTouchKey::Hold);
			if (HoldState.bDown)
			{
				HoldState.Events[IE_Released]++;
				HoldState.Position1 = CurrentPosition;
				HoldState.DownTime = DownTime;
			}

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

			CurTouchLevel = ETouchEventLevel::OnePoint;
		}
	}
}

void UCustomTouchInput::DetectTwoPointsActions(bool bCurrentState, bool bPrevState, float DeltaTime, const FVector2D& CurrentPosition1, const FVector2D& CurrentPosition2)
{
	bTwoPointTouched = bCurrentState;
	if (bCurrentState)
	{
		TwoPointDownTime += DeltaTime;
		if (TwoPointDownTime <= CheckTouchLevelTime)
			return;
			
		if (TwoPointDownTime > CheckTouchLevelTime)
			if (CurTouchLevel > ETouchEventLevel::TwoPoints)
				return;
			else
				CurTouchLevel = ETouchEventLevel::TwoPoints;

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
	}
	else
	{
		if (bPrevState && CurTouchLevel == ETouchEventLevel::TwoPoints)
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

//检测五指状态
void UCustomTouchInput::DetectFivePointsActions(bool bCurrentState, bool bPrevState, float DeltaTime, const TArray<FVector2D>& CurrentPositions)
{
	if (bCurrentState)
	{
		if (ETouchEventLevel::FivePoints > CurTouchLevel)
			CurTouchLevel = ETouchEventLevel::FivePoints;

		if (!bPrevState)
		{
			FivePointDownTime = 0.f;
			FSimpleKeyState& FivePointsState = KeyStateMap.FindOrAdd(EGameTouchKey::FivePoints);
			FivePointsState.Events[IE_Pressed]++;  //五指响应已触发
			FivePointsState.TwoMorePositions = CurrentPositions;
			FivePointsState.DownTime = FivePointDownTime;
		}

		FSimpleKeyState& FivePointsState = KeyStateMap.FindOrAdd(EGameTouchKey::FivePoints);
		if (FivePointsState.bDown)
		{
			FivePointsState.Events[IE_Repeat]++;
			FivePointsState.TwoMorePositions = CurrentPositions;
			FivePointsState.DownTime = FivePointDownTime;
		}

		FivePointDownTime += DeltaTime;
	}
	else
	{
		if (bPrevState)
		{
			FSimpleKeyState& FivePointsState = KeyStateMap.FindOrAdd(EGameTouchKey::FivePoints);
			if (FivePointsState.bDown)
			{
				FivePointsState.Events[IE_Released]++;
				//FivePointsState.TwoMorePositions = CurrentPositions;  //这里不需要重新赋值，因为这里会有小于或大于5的触摸点数目，所以要直接使用上一次检测的点，可能会有1帧的误差
				FivePointsState.DownTime = FivePointDownTime;
			}

			//CurTouchLevel = ETouchEventLevel::OnePoint;
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

	for (const auto& AB : ActionBindings2MP)
	{
		const FSimpleKeyState* KeyState = KeyStateMap.Find(AB.Key);

		if(KeyState && KeyState->Events[AB.KeyState] > 0)
		{
			AB.ActionDelegate.ExecuteIfBound(KeyState->TwoMorePositions, KeyState->DownTime);
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