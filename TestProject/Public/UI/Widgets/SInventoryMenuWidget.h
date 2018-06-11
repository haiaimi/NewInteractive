// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "SlateBasics.h"
#include "SlateExtras.h"

/**
 * 
 */
class TESTPROJECT_API SInventoryMenuWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SInventoryMenuWidget)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};
