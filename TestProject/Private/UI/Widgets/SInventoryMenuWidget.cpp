// Fill out your copyright notice in the Description page of Project Settings.

#include "SInventoryMenuWidget.h"
#include "SlateOptMacros.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SInventoryMenuWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	.VAlign(EVerticalAlignment::VAlign_Fill)
	.HAlign(EHorizontalAlignment::HAlign_Left)
	[
		SNew(SOverlay)
		+SOverlay::Slot()
		.VAlign(EVerticalAlignment::VAlign_Fill)
		.HAlign(EHorizontalAlignment::HAlign_Left)
		[
			SNew(SBorder)
			//.BorderBackgroundColor(FLinearColor(1.f,1.f,1.f,0.8f))
			[
				SNew(SBox)
				.HAlign(EHorizontalAlignment::HAlign_Center)
				.VAlign(EVerticalAlignment::VAlign_Center)
				.WidthOverride(100)
				.HeightOverride(100)
				[
					SNew(SVerticalBox)
					.Visibility(EVisibility::Visible)
				]
			]
		]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
