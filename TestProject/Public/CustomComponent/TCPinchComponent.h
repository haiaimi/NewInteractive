// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TCPinchComponent.generated.h"

/**
 * �Զ��崥��������Ӧ����--Pinch
 * 1���������ֱ�Ӳ���Actor������Ҫ����Actor
 * 2������Actor�任��CameraΪ���ģ�����Ҫע������CameraDirection�����ʵ�������µ���SetCameraDirecion
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

	/**���ݴ����������InventoryActor��Transform����*/
	void OnPinchUpdated(AActor* TargetActor, const FVector2D Point1, const FVector2D Point2);

	void OnPinchReleased(AActor* TargetActor, const FVector2D Point1, const FVector2D Point2);

	static float ComputeAngleSub(float Angle1, float Angle2);

	void SetCameraDirection(FRotator& InDir);

private:
	/**��ȡ����������֮��ľ��룬����Ļ�߽��γɵĽǶȣ������������γɵĽǶȣ����Լ�����������֮������ĵ�*/
	void GetLengthAndAngle(float& Length, float& Angle, FVector2D& CenterPos);

private:
	/**��ǰ������������,ע��Z������Ϊ�ж�*/
	FVector2D TouchPoints[2];

	/**�����ָ��ķ���*/
	FRotator CameraRotation;

	///Target��ʼ��Transform״̬
	FVector InitialPosition;
	FVector InitialScale;
	FRotator InitialRotation;

	/**�ڰ���ʱ��Target����λ����ָ���µ�λ�õ�ƫ��ֵ*/
	FVector OffsetPos;
	
	/**����ʱ����������ľ���*/
	float InitialLength;
	
	float PreAngle;
	FVector2D InitialCenterPos;
};
