// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "WarSimulateHUD.generated.h"

/**
 * 
 */
UCLASS()
class WARSIMULATE_API AWarSimulateHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AWarSimulateHUD();
	
	virtual void DrawHUD()override;

	virtual void BeginPlay()override;

public:
	bool IsInUI();

	bool IsInventoryWidgetValid();

	/**菜单是否在显示状态*/
	bool IsMenuShow();

	/**显示菜单*/
	void ShowMenu(bool ShouldShow);

	void DrawCustomDebugLine(bool bDrawDebug, const FVector2D& Point1, const FVector2D& Point2);

	void SpawnNewWidget();

	/**绘制小地图*/
	void DrawMiniMap();

private:
	TSharedPtr<class SInventoryMenuWidget> InventoryWidget;

	/**长按弹出的菜单*/
	//TSharedPtr<class SMultiSelectWidget> MultiSelectWidget;

	///下面的变量是Debug所需要的
	uint8 bDrawDebugLine : 1;

	FVector2D TouchPoints[2];

	/**小地图捕捉*/
	class AOverLookMiniMapCapture* MiniMapCapture;
};
