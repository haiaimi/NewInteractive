// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomTouchInput.h"
#include "Gameplay/MainController.h"
#include "GameFramework/PlayerInput.h"


UCustomTouchInput::UCustomTouchInput(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	,PreTouched(false)
	,Touch0DownTime(0.f)
{

}

void UCustomTouchInput::UpdateInputStates(float DeltaTime)
{
	UpdateGameKeys(DeltaTime);
	ProcessKeyStates(DeltaTime);
}

void UCustomTouchInput::UpdateGameKeys(float DeltaTime)
{
	if (!GetOuter())return;
	AMainController* Controller = Cast<AMainController>(GetOuter());
	
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

	DetectOnePointActions(CurrentTouchState & 1, PreTouched & 1, DeltaTime, LocalPosition1, Touch0DownTime);

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
	PreTouched = CurrentTouchState;
}

void UCustomTouchInput::DetectOnePointActions(bool bCurrentState, bool bPrevState, float DeltaTime, const FVector2D& CurrentPosition, float& DownTime)
{
	if (bCurrentState)
	{
		if (!bPrevState)
		{
			DownTime = 0;
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