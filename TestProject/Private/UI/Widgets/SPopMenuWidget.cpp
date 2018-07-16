﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "SPopMenuWidget.h"
#include "SlateOptMacros.h"
#include "TestProjectHelper.h"
#include "SBoxPanel.h"
#include "Children.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SPopMenuWidget::Construct(const FArguments& InArgs)
{
	FSlateBrush* SlateBrush = new FSlateBrush();
	SlateBrush->TintColor = FLinearColor(0.3, 0.3, 0.3, 0.3);
	FSlateBrush* TitleBrush = new FSlateBrush();
	TitleBrush->TintColor = FLinearColor(1.f, 0.804f, 0.219f, 0.5f);

	ButtonAnimHandles.SetNum(4);
	ButtonPaddings.SetNum(ButtonAnimHandles.Num());
	Buttons.SetNum(ButtonAnimHandles.Num());

	TAttribute<FVector2D> MenuSize;

	ChildSlot
	[
		SNew(SOverlay)
		+SOverlay::Slot()
		.VAlign(EVerticalAlignment::VAlign_Top)
		.HAlign(EHorizontalAlignment::HAlign_Center)
		.Padding(600,100,0.f,0.f)
		[
			SNew(SBorder)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.BorderImage(SlateBrush)
			.DesiredSizeScale(TAttribute<FVector2D>(this, &SPopMenuWidget::GetPopMenuSize))
			//.Padding(TAttribute<FMargin>(FMargin(600.f,0.f,0.f,0.f)))
			[
				SNew(SBox)
				.HAlign(EHorizontalAlignment::HAlign_Fill) 
				.VAlign(EVerticalAlignment::VAlign_Fill)
				.WidthOverride(300)
				[
					SAssignNew(ButtonContainer,SVerticalBox)
					+SVerticalBox::Slot()
					.VAlign(EVerticalAlignment::VAlign_Fill)
					.HAlign(EHorizontalAlignment::HAlign_Fill)
					.AutoHeight()
					[
						SNew(SBorder)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.BorderImage(TitleBrush)
							[
								SNew(STextBlock)
								.Text(FText::FromString(FString(TEXT("Details"))))
								.ColorAndOpacity(FSlateColor(FLinearColor(0.f, 0.f, 0.f)))
								.Font(FSlateFontInfo(TEXT("Consolas"), 20))
								.AutoWrapText(true)
							]
						
					]
					+SVerticalBox::Slot()
					.VAlign(EVerticalAlignment::VAlign_Fill)
					.HAlign(EHorizontalAlignment::HAlign_Fill)
					.AutoHeight()
					[
						SAssignNew(Buttons[0], SButton)
						.ButtonColorAndOpacity(FLinearColor(0.4f,0.4f,0.4f,0.1f))
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						.PressMethod(EButtonPressMethod::Type::ButtonRelease)
						.ClickMethod(EButtonClickMethod::Type::MouseDown)
						.TouchMethod(EButtonTouchMethod::Type::DownAndUp)
						.IsFocusable(false)
						//.DesiredSizeScale(TAttribute<FVector2D>(FVector2D(2.f,3.f)))
						[
							SNew(STextBlock)
							.Text(FText::FromString(FString(TEXT("Button1"))))
							.ColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.2f, 0.2f)))
							.Font(FSlateFontInfo(TEXT("Consolas"), 30))
							.AutoWrapText(true)
						]
					]
					+ SVerticalBox::Slot()
					.VAlign(EVerticalAlignment::VAlign_Fill)
					.HAlign(EHorizontalAlignment::HAlign_Fill)
					.AutoHeight()
					[
						SAssignNew(Buttons[1], SButton)
						.ButtonColorAndOpacity(FLinearColor(0.4f,0.4f,0.4f,0.1f))
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						.PressMethod(EButtonPressMethod::Type::ButtonRelease)
						.ClickMethod(EButtonClickMethod::Type::MouseDown)
						.TouchMethod(EButtonTouchMethod::Type::DownAndUp)
						.IsFocusable(false)
						//.DesiredSizeScale(TAttribute<FVector2D>(FVector2D(2.f,3.f)))
						[
							SNew(STextBlock)
							.Text(FText::FromString(FString(TEXT("Button2"))))
							.ColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.2f, 0.2f)))
							.Font(FSlateFontInfo(TEXT("Consolas"), 30))
							.AutoWrapText(true)
						]
					]
					+SVerticalBox::Slot()
					.VAlign(EVerticalAlignment::VAlign_Fill)
					.HAlign(EHorizontalAlignment::HAlign_Fill)
					.AutoHeight()
					[
						SAssignNew(Buttons[2], SButton)
						.ButtonColorAndOpacity(FLinearColor(0.4f,0.4f,0.4f,0.1f))
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						.PressMethod(EButtonPressMethod::Type::ButtonRelease)
						.ClickMethod(EButtonClickMethod::Type::MouseDown)
						.TouchMethod(EButtonTouchMethod::Type::DownAndUp)
						.IsFocusable(false)
						//.DesiredSizeScale(TAttribute<FVector2D>(FVector2D(2.f,3.f)))
						[
							SNew(STextBlock)
							.Text(FText::FromString(FString(TEXT("Button3"))))
							.ColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.2f, 0.2f)))
							.Font(FSlateFontInfo(TEXT("Consolas"), 30))
							.AutoWrapText(true)
						]
					]
					+ SVerticalBox::Slot()
					.VAlign(EVerticalAlignment::VAlign_Fill)
					.HAlign(EHorizontalAlignment::HAlign_Fill)
					.AutoHeight()
					[
						SAssignNew(Buttons[3], SButton)
						.ButtonColorAndOpacity(FLinearColor(0.4f, 0.4f, 0.4f, 0.1f))
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						.PressMethod(EButtonPressMethod::Type::DownAndUp)
						[
							SNew(STextBlock)
							.Text(FText::FromString(FString(TEXT("Button4"))))
							.ColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.2f, 0.2f)))
							.Font(FSlateFontInfo(TEXT("Consolas"), 30))
							.AutoWrapText(true)
						]
					]
				]
			]
		]
	];

	SetupAnimation();
}

void SPopMenuWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	for (int32 i = 0; i < ButtonPaddings.Num(); ++i)
	{
		float ButtonAnimProgress = ButtonAnimHandles[i].GetLerp();
		//获取当前按键的Padding
		FMargin CurPadding = FMargin(0.f, (30.f * i) * (1 - ButtonAnimProgress), 0.f, 0.f);
		FChildren* Children = ButtonContainer->GetChildren();
		//SVerticalBox::FSlot* SlotArray = (SVerticalBox::FSlot*)((TPanelChildren<SBoxPanel::FSlot>*)Children);
		//SVerticalBox::FSlot* CurSlot = &SlotArray[i];

		TPanelChildren<SBoxPanel::FSlot>* PanelChildren = (TPanelChildren<SBoxPanel::FSlot>*)Children;
		SBoxPanel::FSlot& CurButtonSlot = (*PanelChildren)[i + 1];
		CurButtonSlot.SlotPadding = CurPadding;

		FVector2D CurButtonScale = FVector2D(1.f, 2.f - ButtonAnimProgress);
		Buttons[i]->SetDesiredSizeScale(CurButtonScale);
	}
}

void SPopMenuWidget::SetupAnimation()
{
	float StartSeconds = 0.f;
	float DurationSeconds = 0.3f;
	DetailButtonAnimation = FCurveSequence();

	//下面是添加Button位置/尺寸变化的动画曲线
	for (int32 i = 0; i < ButtonAnimHandles.Num(); ++i)
	{
		ButtonAnimHandles[i] = DetailButtonAnimation.AddCurve(StartSeconds + i * 0.1f, DurationSeconds, ECurveEaseFunction::CubicInOut);
	}
	//添加菜单尺寸变化的动画曲线
	PopMenuSizeHandle = DetailButtonAnimation.AddCurve(StartSeconds, DurationSeconds + 0.1f*(ButtonAnimHandles.Num() - 1), ECurveEaseFunction::CubicInOut);      

	DetailButtonAnimation.Play(this->AsShared());
}

FVector2D SPopMenuWidget::GetPopMenuSize()const
{
	const float AnimProgress = PopMenuSizeHandle.GetLerp();
	FVector2D Result = FVector2D(1.5f - 0.5f*AnimProgress, 1.f);

	return Result;
}

float SPopMenuWidget::GetPopMenuHeight() const
{
	float MenuHeight = 0.f;
	const float BaseHeight = Buttons.Num()*Buttons[0]->GetDesiredSize().Y;
	const float MaxHeight = BaseHeight + Buttons.Num()*50.f;

	float MenuHeightProgress = ButtonAnimHandles.Last().GetLerp();
	MenuHeight = FMath::Lerp(MaxHeight, BaseHeight, MenuHeightProgress);

	return MenuHeight;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
