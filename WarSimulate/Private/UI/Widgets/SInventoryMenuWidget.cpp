// Fill out your copyright notice in the Description page of Project Settings.

#include "SInventoryMenuWidget.h"
#include "SlateOptMacros.h"
#include "Engine/Engine.h"
#include "WarSimulateHelper.h"
#include "Earth.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerInput.h"
#include "Gameplay/MainController.h"
#include "GroundCameraComponent.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SInventoryMenuWidget::Construct(const FArguments& InArgs)
{
	HoldTime = 0.f;
	FSlateBrush* SlateBrush = new FSlateBrush();
	SlateBrush->TintColor = FLinearColor(0.3, 0.3, 0.3, 0.3);
	OwnerController = InArgs._OwnerController;

	ChildSlot
	[
		SNew(SOverlay)
		+SOverlay::Slot()
		.VAlign(EVerticalAlignment::VAlign_Fill)
		.HAlign(EHorizontalAlignment::HAlign_Left)
		.Padding(TAttribute<FMargin>(this, &SInventoryMenuWidget::GetMenuOffset))
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
				.HeightOverride(1000)
				[
					SNew(SVerticalBox)
					+SVerticalBox::Slot()
					.VAlign(EVerticalAlignment::VAlign_Center)
					.HAlign(EHorizontalAlignment::HAlign_Center)
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString(FString(TEXT("TEXT"))))
						.ColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.2f, 0.2f)))
						.Font(FSlateFontInfo(TEXT("Roboto"),40))
						.AutoWrapText(true)
					]
					+ SVerticalBox::Slot()
					.VAlign(EVerticalAlignment::VAlign_Center)
					.HAlign(EHorizontalAlignment::HAlign_Center)
					.AutoHeight()
					//.Padding(0.f,100.f,0.f,0.f)
					[
						SNew(STextBlock)
						.Text(FText::FromString(FString(TEXT("TEXT"))))
						.ColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.2f, 0.2f)))
						.Font(FSlateFontInfo(TEXT("Roboto"), 40))
						.AutoWrapText(true)
					]
					+SVerticalBox::Slot()
					.VAlign(EVerticalAlignment::VAlign_Fill)
					.HAlign(EHorizontalAlignment::HAlign_Fill)
					.AutoHeight()
					//.Padding(0.f, 50.f, 0.f, 0.f)
					[
						SNew(SButton)
						.ButtonColorAndOpacity(FLinearColor(0.4f,0.4f,0.4f,0.1f))
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						.OnPressed(this, &SInventoryMenuWidget::OnPressed)
						.OnReleased(this, &SInventoryMenuWidget::OnReleased)
						//.OnClicked(this, &SInventoryMenuWidget::OnClicked)
						.PressMethod(EButtonPressMethod::Type::ButtonPress)
						.ClickMethod(EButtonClickMethod::Type::MouseDown)
						//.TouchMethod(EButtonTouchMethod::Type::)
						.IsFocusable(false)
						//.DesiredSizeScale(TAttribute<FVector2D>(FVector2D(2.f,3.f)))
						[
							SNew(STextBlock)
							.Text(FText::FromString(FString(TEXT("Earth"))))
							.ColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.2f, 0.2f)))
							.Font(FSlateFontInfo(TEXT("Roboto"), 40))
							.AutoWrapText(true)
						]
					]
					+ SVerticalBox::Slot()
					.VAlign(EVerticalAlignment::VAlign_Fill)
					.HAlign(EHorizontalAlignment::HAlign_Fill)
					.AutoHeight()
					[
						SNew(SButton)
						.ButtonColorAndOpacity(FLinearColor(0.4f, 0.4f, 0.4f, 0.1f))
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						.OnReleased(this, &SInventoryMenuWidget::LoadLandscape)
						.PressMethod(EButtonPressMethod::Type::DownAndUp)
						[
							SNew(STextBlock)
							.Text(FText::FromString(FString(TEXT("Landscape"))))
							.ColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.2f, 0.2f)))
							.Font(FSlateFontInfo(TEXT("Roboto"), 40))
							.AutoWrapText(true)
						]
					]
					+SVerticalBox::Slot()
					.VAlign(EVerticalAlignment::VAlign_Fill)
					.HAlign(EHorizontalAlignment::HAlign_Fill)
					.AutoHeight()
					[
						SAssignNew(EditableText, SEditableText)
						.HintText(FText::FromString(TEXT("Input")))
						.ColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.2f, 0.2f)))
						.Font(FSlateFontInfo(TEXT("Roboto"), 40))
						.OnTextCommitted(this,&SInventoryMenuWidget::OnTextCommitted)
						.VirtualKeyboardTrigger(EVirtualKeyboardTrigger::OnAllFocusEvents)
					]
				]
			]
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Bottom)
		.Padding(TAttribute<FMargin>(this, &SInventoryMenuWidget::GetPreViewButtonOffset))
		[
			SNew(SButton)
			.ButtonColorAndOpacity(FLinearColor(0.4f,0.4f,0.4f,0.1f))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			.OnClicked(this, &SInventoryMenuWidget::StopPreview)
			.ClickMethod(EButtonClickMethod::Type::MouseUp)
			[	
				SNew(STextBlock)
				.Text(FText::FromString(FString(TEXT("结束预览"))))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.2f, 0.2f)))
				.Font(FSlateFontInfo(TEXT("Roboto"), 40))			]
		]
	];

	bShowMenu = false;
	SetupAnimation();
}

void SInventoryMenuWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (HoldTime >= 0.7f)
	{
		OwnerController->StartPreview();
		PlayPreviewButtonAnimation(true);
		HoldTime = 0.f;
	}

	if (HoldTime > 0)
		HoldTime += InDeltaTime;
}

void SInventoryMenuWidget::OnPressed()
{
	if (GEngine && OwnerController.IsValid() && !IsInUI)
	{
		IsInUI = true;

		OwnerController->SpawnInventoryActors(AEarth::StaticClass());
		HoldTime = 0.001f;
	}
}

void SInventoryMenuWidget::OnReleased()
{
	HoldTime = 0.f;
	IsInUI = false;
	FVector2D CursorPos;
	OwnerController->GetMousePosition(CursorPos.X, CursorPos.Y);
}

void SInventoryMenuWidget::LoadLandscape()
{
	if (OwnerController.IsValid())
	{
		OwnerController->LoadLandscape(TEXT("BattleField"));
	}
}

FReply SInventoryMenuWidget::OnClicked()
{
	if (GEngine && OwnerController.IsValid() && !IsInUI)
	{
		IsInUI = true;

		OwnerController->SpawnInventoryActors(AEarth::StaticClass());
		HoldTime = 0.001f;

		return FReply::Handled();
	}

	return FReply::Handled();
}

FReply SInventoryMenuWidget::StopPreview()
{
	OwnerController->StopPreview();
	PlayPreviewButtonAnimation(false);
	return FReply::Handled();
}

FReply SInventoryMenuWidget::OnEmptyMouseButtonDown(const FGeometry&, const FPointerEvent&)
{
	return FReply::Handled();
}

void SInventoryMenuWidget::OnTextCommitted(const FText& ChangedText, ETextCommit::Type CommitType)
{
	SVirtualKeyboardEntry* VirtualKeyboardEntryPtr = (SVirtualKeyboardEntry*)EditableText.Get();

	IVirtualKeyboardEntry* IVirtualKeyboardEntryPtr = (IVirtualKeyboardEntry*)VirtualKeyboardEntryPtr;
	FSlateApplication::Get().ShowVirtualKeyboard(true, 0, MakeShareable(IVirtualKeyboardEntryPtr));
}

void SInventoryMenuWidget::SetupAnimation()
{
	float StartSeconds = 0.f;
	float SecondSeconds = 0.5f;
	float DurationSeconds = 0.5f;
	InventoryMenuAnimation = FCurveSequence();
	PreviewButtonAnimation = FCurveSequence();

	InventoryMenuIn = InventoryMenuAnimation.AddCurve(StartSeconds, DurationSeconds, ECurveEaseFunction::CubicInOut);
	PreviewButtonIn = PreviewButtonAnimation.AddCurve(StartSeconds, DurationSeconds, ECurveEaseFunction::CubicInOut);
	//InventoryMenuAnimation.Play(this->AsShared());  //先创建智能指针，再播放
}

FMargin SInventoryMenuWidget::GetMenuOffset()const
{
	FMargin Result;

	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	const float MenuWidth = 400.f;
	const float AnimProgess = InventoryMenuIn.GetLerp();
	Result = FMargin(-400.f + AnimProgess * 400.f, 0.f, 0.f, 0.f);

	return Result;
}

FMargin SInventoryMenuWidget::GetPreViewButtonOffset() const
{
	FMargin Result;
	const float AnimProgress = PreviewButtonIn.GetLerp();
	Result = FMargin(0.f, 0.f, 0.f, AnimProgress*150.f - 80.f);
	return Result;
}

void SInventoryMenuWidget::PlayMenuAnimation(bool bShow)
{
	if (bShow && !bShowMenu)    //没有显示菜单的时候才会执行
	{
		bShowMenu = true;

		InventoryMenuAnimation.JumpToStart();
		InventoryMenuAnimation.Play(this->AsShared());
	}
	else if(!bShow && bShowMenu)     //正在显示菜单，需要关闭时
	{
		bShowMenu = false;       //关闭菜单

		InventoryMenuAnimation.Reverse();    //反向播放动画
	}
}

void SInventoryMenuWidget::PlayPreviewButtonAnimation(bool bShow)
{
	if (bShow)
	{
		PreviewButtonAnimation.JumpToStart();
		PreviewButtonAnimation.Play(this->AsShared());
	}
	else
	{
		PreviewButtonAnimation.JumpToEnd();
		PreviewButtonAnimation.Reverse();
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
