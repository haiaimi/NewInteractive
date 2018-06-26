// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomTouchInput.h"
#include "Gameplay/MainController.h"
#include "GameFramework/PlayerInput.h"


UCustomTouchInput::UCustomTouchInput(const FObjectInitializer& ObjectInitializer)
{
	PreTouched = false;
	OwnerController = nullptr;
}

void UCustomTouchInput::UpdateInputStates(float DeltaTime)
{
	UpdateGameKeys(DeltaTime);
	ProcessKeyStates(DeltaTime);
}

void UCustomTouchInput::UpdateGameKeys(float DeltaTime)
{
	if (!OwnerController && GetOuter())
	{
		OwnerController = CastChecked<AMainController>(GetOuter());
	}

	uint32 CurrentTouchState = 0;
	if (OwnerController->PlayerInput->Touches[0].Z != 0)
	{
		CurrentTouchState |= (1 << 0);       //此时有一个触摸点
	}

	FVector Temp = OwnerController->PlayerInput->Touches[0];
	FVector2D TouchPoint = FVector2D(Temp.X, Temp.Y);

	if (CurrentTouchState)
	{
		if (!PreTouched)
		{
			DownTime = 0.f;
			OnePointEvent[IE_Pressed].ExecuteIfBound(TouchPoint, DownTime);
		}

		if (PreTouched)
		{
			DownTime += DeltaTime;
			OnePointEvent[IE_Repeat].ExecuteIfBound(TouchPoint, DownTime);
		}
	}
	else
	{
		if (PreTouched)
		{
			OnePointEvent[IE_Released].ExecuteIfBound(TouchPoint, DownTime);
			DownTime = 0.f;
		}
	}
	PreTouched = CurrentTouchState;
}

void UCustomTouchInput::ProcessKeyStates(float DeltaTime)
{
	
}
