// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TestProjectHUD.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJECT_API ATestProjectHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	ATestProjectHUD();
	
	virtual void DrawHUD()override;

public:
	bool IsInUI();

	bool IsInventoryWidgetValid();

	/**菜单是否在显示状态*/
	bool IsMenuShow();

	/**显示菜单*/
	void ShowMenu(bool ShouldShow);

	void DrawCustomDebugLine(bool bDrawDebug, const FVector2D& Point1, const FVector2D& Point2);

	void SpawnNewWidget();

private:
	TSharedPtr<class SInventoryMenuWidget> InventoryWidget;

	/**长按弹出的菜单*/
	TSharedPtr<class SPopMenuWidget> PopMenuWidget;

	///下面的变量是Debug所需要的
	uint8 bDrawDebugLine : 1;

	FVector2D TouchPoints[2];
};
