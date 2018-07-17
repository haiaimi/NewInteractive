// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Widgets/SCompoundWidget.h"

static const float PopMenuWidth = 157.f;     //PopMenu的宽度，注意是相对于标准分辨率（1080p）
static const float PopButtonHeight = 60.f;     //单个按键高度，注意是相对于标准分辨率（1080p）

struct FPopMenuInfo
{
	/**Menu的位置*/
	FMargin MenuPos;

	/**横向对齐方式*/
	EHorizontalAlignment HorizontalAlignType;    

	///还有其他相关信息，如按键数目等等
	/**按键数量*/
	int32 ButtonsNum;

public:

	FPopMenuInfo(FMargin& InMargin, EHorizontalAlignment InAlignType, int32 InButtonNum)
	{
		MenuPos = InMargin;
		HorizontalAlignType = InAlignType;
		ButtonsNum = InButtonNum;
	}

	FPopMenuInfo()
	{
		MenuPos = FMargin(0.f, 0.f, 0.f, 0.f);
		HorizontalAlignType = EHorizontalAlignment::HAlign_Left;
		ButtonsNum = 4;
	}
};
/**
 *  长按呼出的快捷菜单，可以有根据选中的物体自定义按键功能
 */
class SPopMenuWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPopMenuWidget)
	{}
	SLATE_ARGUMENT(FPopMenuInfo, InMenuInfo)
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
	TArray<TSharedPtr<class STextBlock>> TextBlocks;

	TArray<TSharedPtr<class SButton>> Buttons;

	TSharedPtr<SVerticalBox> ButtonContainer;
};
