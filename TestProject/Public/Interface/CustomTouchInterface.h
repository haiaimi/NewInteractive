// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CustomTouchInterface.generated.h"

UENUM(BlueprintType)
namespace ECustomTouchType
{
	enum Type
	{
		Tap_1P,
		DoubleTap_1P,
		Drag_1P,
		RotateSwipe_1P,
		MoveRotateScalePinch_2P,
	};
}

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class TESTPROJECT_API UCustomTouchInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * ���д�������Ľӿڣ����ڶ������й�ͬ����
 */
class ICustomTouchInterface
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	UFUNCTION(BlueprintNativeEvent)
	void GetTouchTypes(TArray<TEnumAsByte<ECustomTouchType::Type>>& OutTypes);

	UFUNCTION(BlueprintNativeEvent)
	void AddTouchTypes(ECustomTouchType::Type InType);

	UFUNCTION(BlueprintNativeEvent)
	void CanSuitTargetTouchType(TEnumAsByte<ECustomTouchType::Type>& TargetTouchType, bool& Out);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InterfaceFunction(ECustomTouchType::Type InType);
	
	/**�ö�����֧�ֵĴ�������*/
	TArray<TEnumAsByte<ECustomTouchType::Type>> SupportTouchType;
};
