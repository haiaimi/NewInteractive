// Fill out your copyright notice in the Description page of Project Settings.

#include "SMultiSelectWidget.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SMultiSelectWidget::Construct(const FArguments& InArgs)
{
	FSlateBrush* SlateBrush = new FSlateBrush();
	SlateBrush->TintColor = FLinearColor(0.3, 0.3, 0.3, 0.3f);

	ChildSlot
	[
		SNew(SOverlay)
		+SOverlay::Slot()
		.VAlign(InArgs._SelectWidgetInfo.VerticalAlignment)
		.HAlign(InArgs._SelectWidgetInfo.HorizontalAlignment)
        .Padding(InArgs._SelectWidgetInfo.Padding)
		[
			SNew(SBorder)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.BorderImage(SlateBrush)
			.Padding(FMargin(1.f,1.f,1.f,1.f))
			[
				SNew(SBox)
				.HAlign(EHorizontalAlignment::HAlign_Fill) 
				.VAlign(EVerticalAlignment::VAlign_Fill)
				.WidthOverride(InArgs._SelectWidgetInfo.Width)
				.HeightOverride(InArgs._SelectWidgetInfo.Height)
				[
					SNew(SBorder)
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
				]
			]
		]
	];
	
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
