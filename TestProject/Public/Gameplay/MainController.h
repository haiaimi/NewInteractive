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

	virtual void ProcessPlayerInput(const float DeltaTime, const bool bGamePaused)override;

public:
	void ToggleTableMaterial();

	/**鼠标点击一些物体然后实现拖拽*/
	void DragSomeThing();

	void StopDrag();

	void SpawnInventoryActors(UClass* SpawnedActor);

	void QuitGame();

	/**储物柜开关*/
	void SwitchLocker();

	/**判断鼠标指针是否在菜单区域*/
	bool DoesCursorInMenu();

	void LoadLandscape(FName const LevelName);

	void DragLandscapePressed(const FVector2D& Point, float DownTime);

	void DragLandscapeReleased(const FVector2D& Point, float DownTime);

	void DragLandscapeUpdate(const FVector2D& Point, float DownTime);

	/**判断当前状态是否可以拖拽地形，当用户在拖拽物体和控制UI时不可以拖拽*/
	bool CanDragLanscape();

private:
	class AGroundSpectatorPawn* GetGroundSpectatorPawn()const;

	class UGroundCameraComponent* GetGroundCamera()const;

private:
	/**用于桌面的材质库*/
	UPROPERTY()
	TArray<UMaterialInterface*> MaterialInventory;

	/**Controller对应的桌面*/
	UPROPERTY()
	class ATable* CurTable;

	/**当前的储物柜*/
	UPROPERTY()
	class ALocker* CurLocker;

	/**当前拖拽的物体*/
	UPROPERTY()
	class AInventoryActor* CurDragThing;

	/**当前在菜单中生成的物体*/
	class AInventoryActor* CurMenuSpawnThing;

	int32 MaterialIndex;

	/**生成Locker的定时器*/
	FTimerHandle SpawnLockerHandle;

	UPROPERTY()
	class UCustomTouchInput* InputHandle;

};
