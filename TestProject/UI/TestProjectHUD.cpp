// Fill out your copyright notice in the Description page of Project Settings.

#include "TestProjectHUD.h"
#include "UI/Widgets/SInventoryMenuWidget.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "TestProjectHelper.h"




ATestProjectHUD::ATestProjectHUD()
{

}

void ATestProjectHUD::DrawHUD()
{
	Super::DrawHUD();

	//if (!InventoryWidget.IsValid())
	//{
	//	SAssignNew(InventoryWidget, SInventoryMenuWidget);

	//	if (InventoryWidget.IsValid() && GEngine)
	//	{
	//		GEngine->GameViewport->AddViewportWidgetContent(
	//			SNew(SWeakWidget)
	//			.PossiblyNullContent(InventoryWidget.ToSharedRef()),
	//			0
	//		);

	//		//InventoryWidget->SetVisibility(EVisibility::Visible);
	//	}
	//}

	//if (InventoryWidget.IsValid())
	//{
	//	if (InventoryWidget->IsHovered())
	//		TestProjectHelper::Debug_ScreenMessage(TEXT("Hovered"));
	//}
}
