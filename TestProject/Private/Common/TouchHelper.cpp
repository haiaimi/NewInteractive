// Fill out your copyright notice in the Description page of Project Settings.

#include "TouchHelper.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"

void TouchHelper::GetTouchInputInfo(class APlayerController* InPlayer, TArray<FVector>& OutTouchPoint)
{
	OutTouchPoint.Empty();
	const FVector* Touches = InPlayer->PlayerInput->Touches;
	
	for (int32 i = 0; i < 11; ++i)
	{
		if (Touches[i].Z != 0)
			OutTouchPoint.Add(Touches[i]);
		else 
			break;
	}
}

bool TouchHelper::IsTouchTypeContained(UObject* TouchComponent, AActor* TargetActor, TEnumAsByte<ECustomTouchType::Type> ComponentTouchType)
{
	bool Result = false;
	if (TargetActor->GetClass()->ImplementsInterface(UCustomTouchInterface::StaticClass()))
	{
		ICustomTouchInterface::Execute_CanSuitTargetTouchType(TargetActor, ComponentTouchType, Result);
	}

	return Result;
}
