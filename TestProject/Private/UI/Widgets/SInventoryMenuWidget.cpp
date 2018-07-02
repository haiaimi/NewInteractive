// Fill out your copyright notice in the Description page of Project Settings.

#include "SInventoryMenuWidget.h"
#include "SlateOptMacros.h"
#include "Engine/Engine.h"
#include "TestProjectHelper.h"
#include "Earth.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerInput.h"
#include "Gameplay/MainController.h"


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
				.HeightOverride(600)
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
						.TouchMethod(EButtonTouchMethod::Type::DownAndUp)
						.IsFocusable(false)
						[
							SNew(STextBlock)
							.Text(FText::FromString(FString(TEXT("Earth"))))
							.ColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.2f, 0.2f)))
							.Font(FSlateFontInfo(TEXT("TestFont"), 40))
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
							.Font(FSlateFontInfo(TEXT("TestFont"), 40))
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
						.Font(FSlateFontInfo(TEXT("TestFont"), 40))
						.OnTextCommitted(this,&SInventoryMenuWidget::OnTextCommitted)
						.VirtualKeyboardTrigger(EVirtualKeyboardTrigger::OnAllFocusEvents)
					]
				]
			]
		]
	];

	bShowMenu = false;
	SetupAnimation();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SInventoryMenuWidget::OnPressed()
{
	if (GEngine && OwnerController.IsValid() && !IsInUI)
	{
		IsInUI = true;

		OwnerController->SpawnInventoryActors(AEarth::StaticClass());
		OwnerController->DragSomeThing();
	}
}

void SInventoryMenuWidget::OnReleased()
{
	IsInUI = false;
	FVector2D CursorPos;
	OwnerController->GetMousePosition(CursorPos.X, CursorPos.Y);
	//if(OwnerController->PlayerInput)
	TestProjectHelper::Debug_ScreenMessage(CursorPos.ToString());
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
		OwnerController->DragSomeThing();

		return FReply::Handled();
	}

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

	InventoryMenuIn = InventoryMenuAnimation.AddCurve(StartSeconds, DurationSeconds, ECurveEaseFunction::CubicInOut);
	InventoryMenuAnimation.Play(this->AsShared());  //先创建智能指针，再播放
}

FMargin SInventoryMenuWidget::GetMenuOffset()const
{
	FMargin Result;
	static bool IsFirst = true;      //是否还未触发过

	if (bShowMenu)
		IsFirst = false;
	if (IsFirst)
		return FMargin(-400, 0, 0, 0);

	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	const float MenuWidth = 400.f;
	const float AnimProgess = InventoryMenuIn.GetLerp();
	Result = FMargin(-400.f + AnimProgess * 400.f, 0.f, 0.f, 0.f);
	//TestProjectHelper::Debug_ScreenMessage(FString::Printf(TEXT("%f"), AnimProgess));

	return Result;
}

void SInventoryMenuWidget::PlayOrClosePlayMenuAnim(bool bShow)
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