// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TCPinchComponent.generated.h"

/**
 * 自定义触摸输入响应处理--Pinch
 * 1、该组件中直接操作Actor，所以要传入Actor
 * 2、所有Actor变换以Camera为中心，所以要注意设置CameraDirection，在适当的情况下调用SetCameraDirecion
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESTPROJECT_API UTCPinchComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTCPinchComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OnPinchPressed(class AActor* TargetActor, const FVector2D Point1, const FVector2D Point2);

	/**根据触摸输入更新InventoryActor的Transform属性*/
	void OnPinchUpdated(AActor* TargetActor, const FVector2D Point1, const FVector2D Point2);

	void OnPinchReleased(AActor* TargetActor, const FVector2D Point1, const FVector2D Point2);

	static float ComputeAngleSub(float Angle1, float Angle2);

	void SetCameraDirection(FRotator& InDir);

private:
	/**获取两个触摸点之间的距离，与屏幕边界形成的角度（两个触摸点形成的角度），以及两个触摸点之间的中心点*/
	void GetLengthAndAngle(float& Length, float& Angle, FVector2D& CenterPos);

private:
	/**当前的两个触摸点,注意Z分量仅为判断*/
	FVector2D TouchPoints[2];

	/**摄像机指向的方向*/
	FRotator CameraRotation;

	///Target初始的Transform状态
	FVector InitialPosition;
	FVector InitialScale;
	FRotator InitialRotation;

	/**在按下时，Target中心位置手指按下点位置的偏移值*/
	FVector OffsetPos;
	
	/**按下时两个触摸点的距离*/
	float InitialLength;
	
	float PreAngle;
	FVector2D InitialCenterPos;
};
