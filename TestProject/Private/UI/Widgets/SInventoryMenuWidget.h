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

public:
	void OnPressed();

	void OnReleased();

	void LoadLandscape();

	FReply OnClicked();

	/**���ڴ���Inventory�˵�*/
	FReply OnEmptyMouseButtonDown(const FGeometry&, const FPointerEvent&);

	void OnTextCommitted(const FText& ChangedText, ETextCommit::Type CommitType);

	/**��ʼ������*/
	void SetupAnimation();

	/**��ȡ��������ʱ��Menuƫ��*/
	FMargin GetMenuOffset()const;

	void PlayOrClosePlayMenuAnim(bool bShow);

public:
	TWeakObjectPtr<AMainController> OwnerController;

	/**��ҵ�ǰ����Ƿ���UI��*/
	uint32 IsInUI : 1;

	/**�Ƿ���ʾInventory Menu*/
	uint32 bShowMenu : 1;

	/**����Menu���潥��Ķ���*/
	FCurveHandle InventoryMenuIn;

	/**��������*/
	FCurveSequence InventoryMenuAnimation;

	TSharedPtr<SEditableText> EditableText;
};
