// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Widgets/SCompoundWidget.h"

/**
 *   在框选时用于显示框选范围得控件
 */
class SMultiSelectWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMultiSelectWidget)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};
