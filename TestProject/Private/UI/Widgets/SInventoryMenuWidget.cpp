// Fill out your copyright notice in the Description page of Project Settings.

#include "SInventoryMenuWidget.h"
#include "SlateOptMacros.h"
#include "Engine/Engine.h"
#include "TestProjectHelper.h"
#include "Earth.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SInventoryMenuWidget::Construct(const FArguments& InArgs)
{
	FSlateBrush* SlateBrush = new FSlateBrush();
	SlateBrush->TintColor = FLinearColor(0.3, 0.3, 0.3, 0.3);
	OwnerController = InArgs._OwnerController;

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
			.VAlign(EVerticalAlignment::VAlign_Fill)
			.HAlign(EHorizontalAlignment::HAlign_Fill)
			.OnMouseButtonDown(this, &SInventoryMenuWidget::OnEmptyMouseButtonDown)
			[
				SNew(SBox)
				.VAlign(EVerticalAlignment::VAlign_Fill)
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				.WidthOverride(400)
				.HeightOverride(600)
			]
		]
		+SOverlay::Slot()
		.VAlign(EVerticalAlignment::VAlign_Fill)
		.HAlign(EHorizontalAlignment::HAlign_Left)
		//.Padding(FMargin(-100, 0, 0, 0))
		[
			SNew(SBorder)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.BorderImage(SlateBrush)
			[
				SNew(SBox)
				.HAlign(EHorizontalAlignment::HAlign_Fill) 
				.VAlign(EVerticalAlignment::VAlign_Fill)
				.WidthOverride(400)
				.HeightOverride(600)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.VAlign(EVerticalAlignment::VAlign_Center)
					.HAlign(EHorizontalAlignment::HAlign_Center)
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString(FString(TEXT("TEXT"))))
						.ColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.2f, 0.2f)))
						.Font(FSlateFontInfo(TEXT("TestFont"), 40))
						.AutoWrapText(true)
					]
					+SVerticalBox::Slot()
					.VAlign(EVerticalAlignment::VAlign_Center)
					.HAlign(EHorizontalAlignment::HAlign_Center)
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString(FString(TEXT("TEXT"))))
						.ColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.2f, 0.2f)))
						.Font(FSlateFontInfo(TEXT("TestFont"), 40))
						.AutoWrapText(true)
					]
					+ SVerticalBox::Slot()
					.VAlign(EVerticalAlignment::VAlign_Center)
					.HAlign(EHorizontalAlignment::HAlign_Center)
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString(FString(TEXT("TEXT"))))
						.ColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.2f, 0.2f)))
						.Font(FSlateFontInfo(TEXT("TestFont"), 40))
						.AutoWrapText(true)
					]
					+SVerticalBox::Slot()
					.VAlign(EVerticalAlignment::VAlign_Fill)
					.HAlign(EHorizontalAlignment::HAlign_Fill)
					.AutoHeight()
					[
						SNew(SButton)
						.ButtonColorAndOpacity(FLinearColor(0.4f,0.4f,0.4f,0.1f))
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						//.OnPressed(this,&SInventoryMenuWidget::OnPressed)
						.OnReleased(this, &SInventoryMenuWidget::OnReleased)
						.OnClicked(this, &SInventoryMenuWidget::OnClicked)
						.PressMethod(EButtonPressMethod::Type::ButtonRelease)
						.ClickMethod(EButtonClickMethod::Type::MouseDown)
						[
							SNew(STextBlock)
							.Text(FText::FromString(FString(TEXT("TEST Button"))))
							.ColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.2f, 0.2f)))
							.Font(FSlateFontInfo(TEXT("TestFont"), 40))
							.AutoWrapText(true)
						]
						
					]
				]
			]
		]
		
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SInventoryMenuWidget::OnPressed()
{
	IsInUI = true;
	if (GEngine && OwnerController.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, TEXT("Button Down"));
		TestProjectHelper::Debug_ScreenMessage(this->GetDesiredSize().ToString());

		OwnerController->SpawnInventoryActors(AEarth::StaticClass());
		OwnerController->DragSomeThing();
	}
}

void SInventoryMenuWidget::OnReleased()
{
	IsInUI = false;
}

FReply SInventoryMenuWidget::OnClicked()
{
	if (GEngine && OwnerController.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, TEXT("Button Down"));
		TestProjectHelper::Debug_ScreenMessage(this->GetDesiredSize().ToString());

		OwnerController->SpawnInventoryActors(AEarth::StaticClass());
		OwnerController->DragSomeThing();

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply SInventoryMenuWidget::OnEmptyMouseButtonDown(const FGeometry&, const FPointerEvent&)
{
	TestProjectHelper::Debug_ScreenMessage(TEXT("Inventory UI Trigger"));
	return FReply::Handled();
}

void SInventoryMenuWidget::SetupAnimation()
{
	float StartSeconds = 0.f;
	float SecondSeconds = 0.5f;
	float DurationSeconds = 0.5f;
	InventoryMenuAnimation = FCurveSequence();

	InventoryMenuIn = InventoryMenuAnimation.AddCurve(StartSeconds, DurationSeconds, ECurveEaseFunction::QuadInOut);
}

FMargin SInventoryMenuWidget::GetMenuOffset()
{

}
