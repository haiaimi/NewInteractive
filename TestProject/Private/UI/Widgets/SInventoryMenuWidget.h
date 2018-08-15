// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Gameplay/MainController.h"

/**
 * 
 */
class TESTPROJECT_API SInventoryMenuWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SInventoryMenuWidget)
	{}
	SLATE_ARGUMENT(TWeakObjectPtr<AMainController>, OwnerController)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)override;

public:
	void OnPressed();

	void OnReleased();

	void LoadLandscape();

	FReply OnClicked();

	/**结束预览按钮*/
	FReply StopPreview();

	/**用于触发Inventory菜单*/
	FReply OnEmptyMouseButtonDown(const FGeometry&, const FPointerEvent&);

	void OnTextCommitted(const FText& ChangedText, ETextCommit::Type CommitType);

	/**初始化动画*/
	void SetupAnimation();

	/**获取动画播放时的Menu偏移*/
	FMargin GetMenuOffset()const;

	FMargin GetPreViewButtonOffset()const;

	void PlayMenuAnimation(bool bShow);

	void PlayPreviewButtonAnimation(bool bShow);

public:
	TWeakObjectPtr<AMainController> OwnerController;

	/**玩家当前鼠标是否在UI内*/
	uint32 IsInUI : 1;

	/**是否显示Inventory Menu*/
	uint32 bShowMenu : 1;

	/**用于Menu界面渐入的动画*/
	FCurveHandle InventoryMenuIn;

	/**结束预览按键动画*/
	FCurveHandle PreviewButtonIn;

	/**动画序列*/
	FCurveSequence InventoryMenuAnimation;

	FCurveSequence PreviewButtonAnimation;

	TSharedPtr<SEditableText> EditableText;

	/**长按时间*/
	float HoldTime;
};
