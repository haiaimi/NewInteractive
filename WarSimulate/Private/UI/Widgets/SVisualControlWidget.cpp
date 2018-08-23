// Fill out your copyright notice in the Description page of Project Settings.

#include "SVisualControlWidget.h"
#include "SlateOptMacros.h"
#include "Materials/MaterialInterface.h"
#include "OriginHelper.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Gameplay/PlatformController.h"
#include "FlightPlatform.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SVisualControlWidget::Construct(const FArguments& InArgs)
{
	VisualControlMaterial = nullptr;
	OwnerController = InArgs._OwnerController;
	UMaterialInterface* Material = nullptr;
	Material = LoadObject<UMaterialInterface>(OwnerController.Get(), TEXT("/Game/AircraftHud/Material/MI_Hud_FlatWidgetMasked"));
	//OriginHelper::Debug_ScreenMessage(TEXT("查找材质"),20);
	if (Material)
	{
		//OriginHelper::Debug_ScreenMessage(TEXT("材质实例"),20);
		if (!Cast<UMaterialInstanceDynamic>(Material))
		{
			UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(Material, OwnerController.Get());
			if (DynamicMaterialInstance)
			{
				VisualControlMaterial = DynamicMaterialInstance;
				//OriginHelper::Debug_ScreenMessage(TEXT("动态材质"),20);
			}
		}
	}
	TAttribute<TOptional<FTransform2D>> TransformAttribute;
	TransformAttribute.BindUObject(OwnerController.Get()->GetControlPlatform(), &AFlightPlatform::GetSlateRenderTransform);
	FSlateBrush* VisualControlBrush = new FSlateBrush;
	VisualControlBrush->SetResourceObject(VisualControlMaterial);

	ChildSlot
	[
		// Populate the widget
		SNew(SOverlay)
		+SOverlay::Slot()
		.VAlign(EVerticalAlignment::VAlign_Center)
		.HAlign(EHorizontalAlignment::HAlign_Center)
		//.Padding(TAttribute<FMargin>(this, &SInventoryMenuWidget::GetMenuOffset))
		[
			SNew(SBorder)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.BorderImage(VisualControlBrush)
			.RenderTransform(TOptional<FTransform2D>(FTransform2D(FQuat2D(10.f))))
			[
				SNew(SBox)
				.VAlign(EVerticalAlignment::VAlign_Center)
				.HAlign(EHorizontalAlignment::HAlign_Center)
				.HeightOverride(800.f)
				.WidthOverride(800.f)
			]
		]
	];

	//SetRenderTransform(TransformAttribute);
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
