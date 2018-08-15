// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GroundCameraComponent.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJECT_API UGroundCameraComponent : public UCameraComponent
{
	GENERATED_BODY()
	
public:
	UGroundCameraComponent();

	/**从该相机类中获取数据*/
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)override; 

public:
	// 相机的移动，其主要就是由SpectatorPawn控制
	void MoveRight(float Val);

	void MoveForward(float Val);

	/**获取玩家Controller*/
	APlayerController* GetPlayerController();

	void StartSwipe(const FVector2D& InPoint, float DownTime);

	void UpdateSwipe(const FVector2D& InPoint, float DownTime);

	void EndSwipe(const FVector2D& InPoint, float DownTime);

	void OnPinchStart(const FVector2D& InPoint1, const FVector2D& InPoint2, float DownTime);

	void OnPinchUpdate(class UCustomTouchInput* InputHandle, const FVector2D& InPoint1, const FVector2D& InPoint2, float DownTime);

	void OnPinchEnd(const FVector2D& InPoint1, const FVector2D& InPoint2, float DownTime);

	/**开启镜头模糊模式*/
	void BlurMode(bool bBlur);

public:
	/**摄像机观察的方向*/
	FRotator LookRotation;    

private:
	/**该点就是中心点（就是手指指向的世界位置），后续更新的点都围绕这个点*/
	FVector StartSwipePos;

	///下面是拖拽阻尼效果的全局变量
	FVector DecelerateSpeed;

	/**用来记录触摸经过的轨迹*/
	TArray<FVector> TracePoints;     

	//最终更新位置的时间
	float LastUpdateTime;

	uint8 bDecelerate : 1;

	///下面是Pinch缩放相关
	//当前整体缩放状态
	float ZoomAlpha;

	//进行缩放操作时的比例
	float InitialPinchAlpha;

	uint8 bInPinch : 1;
};
