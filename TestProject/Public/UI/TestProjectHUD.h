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

	/**�˵��Ƿ�����ʾ״̬*/
	bool IsMenuShow();

	/**��ʾ�˵�*/
	void ShowMenu(bool ShouldShow);

	void DrawCustomDebugLine(bool bDrawDebug, const FVector2D& Point1, const FVector2D& Point2);

private:
	TSharedPtr<class SInventoryMenuWidget> InventoryWidget;

	///����ı�����Debug����Ҫ��
	uint8 bDrawDebugLine : 1;

	FVector2D TouchPoints[2];
};
