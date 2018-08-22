// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WarSimulateHUD.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "OriginHelper.h"
#include "Gameplay/PlatformController.h"
#include "Widgets/SInventoryMenuWidget.h"
#include "DrawDebugHelpers.h"
#include "SPopMenuWidget.h"
#include "SMultiSelectWidget.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture.h"
#include "Components/SceneCaptureComponent2D.h"
#include "CanvasItem.h"
#include "Engine/Canvas.h"
#include "OverLookMiniMapCapture.h"


AWarSimulateHUD::AWarSimulateHUD()
	:bDrawDebugLine(false)
{
}

void AWarSimulateHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!InventoryWidget.IsValid() && GEngine)
	{
		if (APlatformController* OwnerController = Cast<APlatformController>(GetOwningPlayerController()))
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

	DrawMiniMap();
}

void AWarSimulateHUD::BeginPlay()
{
	Super::BeginPlay();

	MiniMapCapture = GetWorld()->SpawnActor<AOverLookMiniMapCapture>(PlayerOwner->GetFocalLocation() + FVector(0.f, 0.f, 1000.f), FRotator::ZeroRotator);
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

void AWarSimulateHUD::DrawMiniMap()
{
	if (MiniMapCapture->GetCaptureComponent2D()->TextureTarget)
	{
		MiniMapCapture->GetCaptureComponent2D()->UpdateContent();
		float MapWidth = MiniMapCapture->MiniMapWidth;
		float MapHeight = MiniMapCapture->MiniMapHeight;

		FCanvasTileItem MinimapTileItem(FVector2D::ZeroVector, FVector2D::ZeroVector, FLinearColor::White);
		MinimapTileItem.Size = FVector2D(MapWidth, MapHeight);
		MinimapTileItem.Texture = MiniMapCapture->GetCaptureComponent2D()->TextureTarget->Resource;
		MinimapTileItem.BlendMode = ESimpleElementBlendMode::SE_BLEND_Opaque;       //非透明混合
		Canvas->DrawItem(MinimapTileItem, Canvas->ClipX - MapWidth, Canvas->ClipY - MapHeight);    //在hud中绘制地图
	}
}
