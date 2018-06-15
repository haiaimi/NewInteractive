// Fill out your copyright notice in the Description page of Project Settings.

#include "TestProjectHUD.h"
#include "UI/Widgets/SInventoryMenuWidget.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "TestProjectHelper.h"
#include "MainController.h"




ATestProjectHUD::ATestProjectHUD()
{
	
}

void ATestProjectHUD::DrawHUD()
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
}

bool ATestProjectHUD::IsInUI()
{
	if (InventoryWidget.IsValid())
	{
		return InventoryWidget->IsInUI;
	}

	return false;
}
