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
	Material = LoadObject<UMaterialInterface>(OwnerController.Get(), TEXT("/Game/AircraftHud/Material/M_HudWidget_Inst"));
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
			}
		}
	}
	TAttribute<TOptional<FTransform2D>> TransformAttribute;
	TransformAttribute.BindUObject(OwnerController.Get(), &APlatformController::GetVisualControlRenderTrans);
	FSlateBrush* VisualControlBrush = new FSlateBrush;
	VisualControlBrush->SetResourceObject(VisualControlMaterial);

	ChildSlot
	[
		// Populate the widget
		SNew(SOverlay)
		+SOverlay::Slot()
		.VAlign(EVerticalAlignment::VAlign_Center)
		.HAlign(EHorizontalAlignment::HAlign_Center)
		.Padding(FMargin(0.f,0.f,0.f,200.f))
		[
			SNew(SBorder)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.BorderImage(VisualControlBrush)
			//.RenderTransform(/*TOptional<FTransform2D>(FTransform2D(FQuat2D(PI/3)))*/TransformAttribute)     //控件不需要旋转
			.RenderTransformPivot(FVector2D(0.5f, 0.5f))
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

void SVisualControlWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

void SVisualControlWidget::SetControlInput(EVisualControlInputs::Type InputType, float Value)
{
	if (VisualControlMaterial)
	{
		switch (InputType)
		{
			case EVisualControlInputs::Roll:
			{
				float RollValue = Value / 360.f - static_cast<int>(Value / 360.f);
				VisualControlMaterial->SetScalarParameterValue(TEXT("Roll"), RollValue);
			}
			break;

			case EVisualControlInputs::Pitch:
			{
				float PitchValue = Value / 360.f - static_cast<int>(Value / 360.f);
				VisualControlMaterial->SetScalarParameterValue(TEXT("Pitch"), -PitchValue);
			}
			break;

			case EVisualControlInputs::Heading:
			{
				float HeadingValue = Value / 360.f - static_cast<int>(Value / 360.f);
				VisualControlMaterial->SetScalarParameterValue(TEXT("Heading"), HeadingValue);
			}
			break;

			/*case EVisualControlInputs::SkidPercent:
				break;
			case EVisualControlInputs::FlightPathUp:
				break;
			case EVisualControlInputs::FlightPathRight:
				break;*/

			case EVisualControlInputs::FlightSpeed:
			{
				const float Speed = (Value / 1000.f)*3.6f;   //时速
				int32 IntegerPart = FMath::FloorToInt(Speed);
				float DecimalPart = Speed - IntegerPart;
				int32 TmpNum = IntegerPart, SingleIndex = 4;
				while (TmpNum)
				{
					int32 SingleNum = TmpNum % 10;
					VisualControlMaterial->SetScalarParameterValue(*FString::Printf(TEXT("Digits0%d"), SingleIndex), SingleNum);
					SingleIndex--;
					TmpNum /= 10;
				}
				VisualControlMaterial->SetScalarParameterValue(TEXT("DeltaSpeed"), Speed / 2000.f);
			}
			break;

			case EVisualControlInputs::FlightAltitude:
			{
				const float Altitude = Value / 100.f + 1000.f;
				int32 IntegerPart = FMath::FloorToInt(Altitude);
				int TmpNum = IntegerPart, SingleIndex = 4;
				while(TmpNum)
				{
					int32 SingleNum = TmpNum % 10;
					VisualControlMaterial->SetScalarParameterValue(*FString::Printf(TEXT("Digits1%d"), SingleIndex), SingleNum);
					SingleIndex--;
					TmpNum /= 10;
				}

				VisualControlMaterial->SetScalarParameterValue(TEXT("DeltaAlt"), Altitude / 2000.f);
			}
			break;

		default:
			break;
		}
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

