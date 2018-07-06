// Fill out your copyright notice in the Description page of Project Settings.

#include "STCDebugWidget.h"
#include "SlateOptMacros.h"
#include "SCanvas.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void STCDebugWidget::Construct(const FArguments& InArgs)
{
	
	ChildSlot
	[
		SAssignNew(Canvas, SCanvas)
		+SCanvas::Slot()
		.VAlign(EVerticalAlignment::VAlign_Fill)
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		// Populate the widget
	];
	
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

//int32 STCDebugWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
//{
//
//}
