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
		.VAlign(EVerticalAlignment::VAlign_Top)
		.HAlign(EHorizontalAlignment::HAlign_Left)
        .Padding(FMargin(0.f,0.f,0.f,0.f))
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
				.WidthOverride(1200)
				.HeightOverride(400)
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
