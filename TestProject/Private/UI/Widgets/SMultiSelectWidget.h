// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Widgets/SCompoundWidget.h"

struct FMultiSelectInfo
{
	/**框选范围的宽及高度*/
	TAttribute<FOptionalSize> Width;
	TAttribute<FOptionalSize> Height;

	/**对齐方式，这个决定了多选框如何变化*/
	EHorizontalAlignment HorizontalAlignment;
	EVerticalAlignment VerticalAlignment;

	/**该控件所在位置*/
	FMargin Padding;

	FMultiSelectInfo() :
		Width(0),
		Height(0),
		HorizontalAlignment(EHorizontalAlignment::HAlign_Left),
		VerticalAlignment(EVerticalAlignment::VAlign_Top),
		Padding(FMargin(0.f, 0.f, 0.f, 0.f))
	{}
};

/**
 *   在框选时用于显示框选范围得控件
 */
class SMultiSelectWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMultiSelectWidget)
	{}
	SLATE_ARGUMENT(FMultiSelectInfo, SelectWidgetInfo)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};
