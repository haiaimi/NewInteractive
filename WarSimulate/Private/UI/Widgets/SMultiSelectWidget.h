// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Widgets/SCompoundWidget.h"

struct FMultiSelectInfo
{
	/**��ѡ��Χ�Ŀ��߶�*/
	TAttribute<FOptionalSize> Width;
	TAttribute<FOptionalSize> Height;

	/**���뷽ʽ����������˶�ѡ����α仯*/
	EHorizontalAlignment HorizontalAlignment;
	EVerticalAlignment VerticalAlignment;

	/**�ÿؼ�����λ��*/
	FMargin Padding;

	FMultiSelectInfo() :
		Width(0),
		Height(0),
		HorizontalAlignment(EHorizontalAlignment::HAlign_Left),
		VerticalAlignment(EVerticalAlignment::VAlign_Top),
		Padding(FMargin(0.f, 0.f, 0.f, 0.f))
	{}
};

/**
 *   �ڿ�ѡʱ������ʾ��ѡ��Χ�ÿؼ�
 */
class SMultiSelectWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMultiSelectWidget)
	{}
	SLATE_ARGUMENT(FMultiSelectInfo, SelectWidgetInfo)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};
