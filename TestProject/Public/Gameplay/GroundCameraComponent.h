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

	/**�Ӹ�������л�ȡ����*/
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)override; 

public:
	// ������ƶ�������Ҫ������SpectatorPawn����
	void MoveRight(float Val);

	void MoveForward(float Val);

	/**��ȡ���Controller*/
	APlayerController* GetPlayerController();

public:
	/**������۲�ķ���*/
	FRotator LookRotation;    
};
