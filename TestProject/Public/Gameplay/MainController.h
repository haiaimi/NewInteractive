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

	void StopDrag(FVector2D StopPoint);

	void SpawnInventoryActors(UClass* SpawnedActor);

	void QuitGame();

	/**储物柜开关*/
	void SwitchLocker();

	/**判断指定的点是否在菜单区域*/
	bool DoesPointInMenu(FVector2D Point);

	void LoadLandscape(FName const LevelName);

	void OnHoldPressed(const FVector2D& Point, float DownTime);

	void OnSwipePressed(const FVector2D& Point, float DownTime);

	void OnSwipeReleased(const FVector2D& Point, float DownTime);

	void OnSwipeUpdate(const FVector2D& Point, float DownTime);

	void TapPressed(const FVector2D& Point, float DownTime);

	void DoubleTapPressed(const FVector2D& Point, float DownTime);

	void OnPinchStart(const FVector2D& Point1, const FVector2D& Point2, float DownTime);

	void OnPinchUpdate(const FVector2D& Point1, const FVector2D& Point2, float DownTime);

	void OnPinchEnd(const FVector2D& Point1, const FVector2D& Point2, float DownTime);

	void OpenUI();

	void CloseUI();

	/**开关UI*/
	void SwitchUI(bool bOpenUI);

	/**判断当前状态是否可以拖拽地形，当用户在拖拽物体和控制UI时不可以拖拽*/
	bool CanDragLanscape();

	void ShowHighlight(bool bShow);

	TSharedPtr<class SPopMenuWidget>& GetPopMenu() { return PopMenu; }

	TArray<AActor*>& GetMultiSelectedActors() { return MultiSelectedActors; }

	class UGroundCameraComponent* GetGroundCamera();

	/**预览想要生成的物体*/
	void StartPreview();

	/**结束预览*/
	void StopPreview();

private:
	class AGroundSpectatorPawn* GetGroundSpectatorPawn()const;


private:
	/**Pinch操作的组件*/
	class UTCPinchComponent* PinchComponent;

	/**Tap相关操作的组件*/
	class UTCTapComponent* TapComponent;

	/**五指开关UI的组件*/
	class UTCSwitchUIComponent* SwitchUIComponent;

	/**触摸拖动组件*/
	class UTCDragSwipeComponent* DragSwipeComponent;

	/**多选框组件*/
	class UTCMultiSelectComponent* MultiSelectComponent;

	class APostProcessVolume* PostProcessVoulme;

	/** Pinch组件当前直接控制的Actor*/
	UPROPERTY()
	AActor* TargetActor;

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

	uint8 bShouldSpawnActor : 1;

	uint8 bInPreview : 1;

	UClass* SpawnActor;

	TSharedPtr<class SPopMenuWidget> PopMenu;

	UPROPERTY()
	TArray<AActor*> MultiSelectedActors;
};
