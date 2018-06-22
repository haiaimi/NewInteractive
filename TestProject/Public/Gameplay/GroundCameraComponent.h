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

public:
	/**摄像机观察的方向*/
	FRotator LookRotation;    
};
