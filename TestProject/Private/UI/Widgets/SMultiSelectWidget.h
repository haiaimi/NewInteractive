// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Widgets/SCompoundWidget.h"

/**
 *   �ڿ�ѡʱ������ʾ��ѡ��Χ�ÿؼ�
 */
class SMultiSelectWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMultiSelectWidget)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};
