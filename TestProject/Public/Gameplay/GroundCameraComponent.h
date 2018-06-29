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

	void StartSwipe(const FVector2D& InPoint, float DownTime);

	void UpdateSwipe(const FVector2D& InPoint, float DownTime);

	void EndSwipe(const FVector2D& InPoint, float DownTime);

	void OnPinchStart(const FVector2D& InPoint1, const FVector2D& InPoint2, float DownTime);

	void OnPinchUpdate(class UCustomTouchInput* InputHandle, const FVector2D& InPoint1, const FVector2D& InPoint2, float DownTime);

	void OnPinchEnd(const FVector2D& InPoint1, const FVector2D& InPoint2, float DownTime);

public:
	/**������۲�ķ���*/
	FRotator LookRotation;    

private:
	/**�õ�������ĵ㣨������ָָ�������λ�ã����������µĵ㶼Χ�������*/
	FVector StartSwipePos;

	///��������ק����Ч����ȫ�ֱ���
	FVector DecelerateSpeed;

	/**������¼���������Ĺ켣*/
	TArray<FVector> TracePoints;     

	//���ո���λ�õ�ʱ��
	float LastUpdateTime;

	uint8 bDecelerate : 1;

	///������Pinch�������
	//��ǰ��������״̬
	float ZoomAlpha;

	//�������Ų���ʱ�ı���
	float InitialPinchAlpha;

	uint8 bInPinch : 1;
};
