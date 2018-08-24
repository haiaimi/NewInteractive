// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Widgets/SCompoundWidget.h"
#include "WarSimulateType.h"

class APlatformController;
/**
 * 
 */
class SVisualControlWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SVisualControlWidget)
	{}
	SLATE_ARGUMENT(TWeakObjectPtr<APlatformController>, OwnerController)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)override;

	/**设置*/
	void SetControlInput(EVisualControlInputs::Type InputType, float Value);

private:
	TSharedPtr<class SImage> VisualControlImage;

	/**显控材质*/
	class UMaterialInstanceDynamic* VisualControlMaterial;

	TWeakObjectPtr<APlatformController> OwnerController;
};
