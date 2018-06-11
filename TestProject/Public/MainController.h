// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainController.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJECT_API AMainController : public APlayerController
{
	GENERATED_BODY()

public:
	AMainController();
	
public:
	virtual void BeginPlay()override;
	
	virtual void Tick(float DeltaSeconds)override;

	virtual void SetupInputComponent()override;

public:
	void ToggleTableMaterial();

	/**鼠标点击一些物体然后实现拖拽*/
	void DragSomeThing();

	void StopDrag();

private:
	/**用于桌面的材质库*/
	UPROPERTY()
	TArray<UMaterialInterface*> MaterialInventory;

	/**Controller对应的桌面*/
	UPROPERTY()
	class ATable* CurTable;

	/**当前拖拽的物体*/
	UPROPERTY()
	class AInventoryActor* CurDragThing;

	int32 MaterialIndex;

};
