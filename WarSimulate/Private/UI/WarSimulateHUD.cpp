// Fill out your copyright notice in the Description page of Project Settings.

#include "WarSimulateHUD.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "WarSimulateHelper.h"
#include "Gameplay/MainController.h"
#include "Widgets/SInventoryMenuWidget.h"
#include "DrawDebugHelpers.h"
#include "SPopMenuWidget.h"
#include "SMultiSelectWidget.h"


AWarSimulateHUD::AWarSimulateHUD()
	:bDrawDebugLine(false)
{
}

void AWarSimulateHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!InventoryWidget.IsValid() && GEngine)
	{
		if (AMainController* OwnerController = Cast<AMainController>(GetOwningPlayerController()))
		{
			SAssignNew(InventoryWidget, SInventoryMenuWidget)
			.OwnerController(OwnerController);

			if (InventoryWidget.IsValid())
			{
				GEngine->GameViewport->AddViewportWidgetContent(
					SNew(SWeakWidget)
					.PossiblyNullContent(InventoryWidget.ToSharedRef()),
					0
				);

				InventoryWidget->SetVisibility(EVisibility::SelfHitTestInvisible);
				FSlateApplication::Get().SetKeyboardFocus(InventoryWidget.ToSharedRef());
			}
		}
	}

	/*if (InventoryWidget.IsValid())
	{
		if (InventoryWidget->IsHovered())
			TestProjectHelper::Debug_ScreenMessage(TEXT("Hovered"));
	}*/

	// 绘制debug线
	if (bDrawDebugLine)
	{
		DrawDebugCanvas2DCircle(Canvas, TouchPoints[0], 50.f, 100, FLinearColor::Blue);
		DrawDebugCanvas2DCircle(Canvas, TouchPoints[1], 50.f, 100, FLinearColor::Blue);
		DrawDebugCanvas2DLine(Canvas, TouchPoints[0], TouchPoints[1], FLinearColor::Red);

		FVector2D TwoPointCenter = (TouchPoints[0] + TouchPoints[1]) / 2;   //两个触摸点的中心点位置
		//下面是两个中心点线的方向
		const static FVector2D LineVec1(1.f, 1.f);
		const static FVector2D LineVec2(1.f, -1.f);

		DrawDebugCanvas2DLine(Canvas, TwoPointCenter - LineVec1 * 25, TwoPointCenter + LineVec1 * 25, FLinearColor::Blue);
		DrawDebugCanvas2DLine(Canvas, TwoPointCenter - LineVec2 * 25, TwoPointCenter + LineVec2 * 25, FLinearColor::Blue);
	}
}

bool AWarSimulateHUD::IsInUI()
{
	if (InventoryWidget.IsValid())
	{
		return InventoryWidget->bIsInUI;
	}

	return false;
}

bool AWarSimulateHUD::IsInventoryWidgetValid()
{
	return InventoryWidget.IsValid();
}

bool AWarSimulateHUD::IsMenuShow()
{
	return InventoryWidget->bShowMenu;
}

void AWarSimulateHUD::ShowMenu(bool bShouldShow)
{
	if (InventoryWidget.IsValid())
	{
		InventoryWidget->PlayMenuAnimation(bShouldShow);
	}
}

void AWarSimulateHUD::DrawCustomDebugLine(bool bDrawDebug, const FVector2D& Point1, const FVector2D& Point2)
{
	bDrawDebugLine = bDrawDebug;
	TouchPoints[0] = Point1;
	TouchPoints[1] = Point2;
}

void AWarSimulateHUD::SpawnNewWidget()
{
	/*SAssignNew(MultiSelectWidget, SMultiSelectWidget);

			if (MultiSelectWidget.IsValid())
			{
				GEngine->GameViewport->AddViewportWidgetContent(
					SNew(SWeakWidget).
					PossiblyNullContent(MultiSelectWidget.ToSharedRef()), 
					0
				);
			}*/
}
