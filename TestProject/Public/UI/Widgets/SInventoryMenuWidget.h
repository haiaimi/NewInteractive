// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "MainController.h"

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

	FReply OnClicked();

	/**���ڴ���Inventory�˵�*/
	FReply OnEmptyMouseButtonDown(const FGeometry&, const FPointerEvent&);

	/**��ʼ������*/
	void SetupAnimation();

	FMargin GetMenuOffset();

public:
	TWeakObjectPtr<AMainController> OwnerController;

	/**��ҵ�ǰ����Ƿ���UI��*/
	uint32 IsInUI : 1;

	/**����Menu���潥��Ķ���*/
	FCurveHandle InventoryMenuIn;

	/**��������*/
	FCurveSequence InventoryMenuAnimation;
};