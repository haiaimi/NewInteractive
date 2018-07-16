// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class SPopMenuWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPopMenuWidget)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)override;

private:
	void SetupAnimation();

	FVector2D GetPopMenuSize()const;

	float GetPopMenuHeight()const;

private:
	FCurveSequence DetailButtonAnimation;

	/**各个按钮的动画Handle*/
	TArray<FCurveHandle> ButtonAnimHandles;

	/**菜单尺寸变化的Handle*/
	FCurveHandle PopMenuSizeHandle;

	/**各个Button的位置*/
	TArray<TAttribute<FMargin>> ButtonPaddings;

	TArray<TSharedPtr<class SButton>> Buttons;

	TSharedPtr<SVerticalBox> ButtonContainer;
};
