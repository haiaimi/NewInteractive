// Fill out your copyright notice in the Description page of Project Settings.

#include "TCPinchComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "TestProjectHelper.h"


// Sets default values for this component's properties
UTCPinchComponent::UTCPinchComponent()
	:InitialLength(0.f),
	PreAngle(0.f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;  //����Ҫ��ʱ����
}

// Called when the game starts
void UTCPinchComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UTCPinchComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTCPinchComponent::OnPinchPressed(class AActor* TargetActor, const FVector2D Point1, const FVector2D Point2)
{
	TouchPoints[0] = Point1;
	TouchPoints[1] = Point2;

	//��ȡTargetActor�Ŀռ�����
	InitialPosition = TargetActor->GetActorLocation();
	InitialScale = TargetActor->GetActorScale3D();
	InitialRotation = TargetActor->GetActorRotation();

	GetLengthAndAngle(InitialLength, PreAngle, InitialCenterPos);
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetOwner()))
	{
		FVector WorldPos, WorldDir;
		const FPlane TargetActorPlane(TargetActor->GetActorLocation(), -CameraRotation.Vector());          //TargetActor����ƽ�棬���������
		PlayerController->DeprojectScreenPositionToWorld(InitialCenterPos.X, InitialCenterPos.Y, WorldPos, WorldDir);
		const FVector IntersectPos = FMath::LinePlaneIntersection(WorldPos, WorldPos + 1000.f*WorldDir, TargetActorPlane);  //�����ཻ��λ��
		OffsetPos = TargetActor->GetActorLocation() - IntersectPos;     //�ó�ƫ����
	}
}

void UTCPinchComponent::OnPinchUpdated(AActor* TargetActor, const FVector2D Point1, const FVector2D Point2)
{
	TouchPoints[0] = Point1;
	TouchPoints[1] = Point2;

	float CurLength, CurAngle;
	FVector2D CurCenterPos;
	GetLengthAndAngle(CurLength, CurAngle, CurCenterPos);
	const float ChangedScale = CurLength / InitialLength;   //����ڳ�ʼ��ʱ�ĳߴ�仯����
	const FVector TempScale = TargetActor->GetActorScale3D();   //��ǰActor��С״̬
	const FVector CurScale = InitialScale * ChangedScale;

	if (CurScale.X < 5.f && CurScale.Y < 5.f && CurScale.Z < 5.f && CurScale.X > 0.5f && CurScale.Y > 0.5f && CurScale.Z > 0.5f)
		TargetActor->SetActorScale3D(CurScale);
	else
		TargetActor->SetActorScale3D(TempScale);
	
	TestProjectHelper::Debug_ScreenMessage(InitialScale.ToString());
	TestProjectHelper::Debug_ScreenMessage(TargetActor->GetActorScale3D().ToString());
	//�������ʱ����ײ
	TArray<FOverlapResult> Result;
	if (GetWorld())
	{
		FVector ActorCenter, ActorExtent;
		TargetActor->GetActorBounds(false, ActorCenter, ActorExtent);
		GetWorld()->OverlapMultiByObjectType(Result, ActorCenter, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllStaticObjects), FCollisionShape::MakeBox(ActorExtent));
	}

	for (FOverlapResult& result : Result)
	{
		if (result.Component.IsValid()) //���������ײ���ͻָ���һ����scale
		{
			//TestProjectHelper::Debug_ScreenMessage(TEXT("Overlap!"));
			TargetActor->SetActorScale3D(TempScale);
			break;
		}
	}

	//����TargetActor��λ��
	if (APlayerController* PlayerController = Cast<APlayerController>(GetOwner()))
	{
		FVector WorldPos, WorldDir;
		const FPlane TargetActorPlane(TargetActor->GetActorLocation(), -CameraRotation.Vector());          //TargetActor����ƽ�棬���������
		PlayerController->DeprojectScreenPositionToWorld(CurCenterPos.X, CurCenterPos.Y, WorldPos, WorldDir);
		const FVector IntersectPos = FMath::LinePlaneIntersection(WorldPos, WorldPos + 1000.f*WorldDir, TargetActorPlane);  //�����ཻ��λ��
		TargetActor->SetActorLocation(IntersectPos + OffsetPos, true);    //ע��������ײʱҪ�谭�ƶ�
	}

	//����ʹ����Ԫ��������ת����ΪҪΧ����������ת 
	const float SubAngle = -UTCPinchComponent::ComputeAngleSub(CurAngle, PreAngle);
	FQuat AddedRotation(FRotator(-70.f, 0.f, 0.f).Vector(), FMath::DegreesToRadians(SubAngle));
	//TestProjectHelper::Debug_ScreenMessage(FString::Printf(TEXT("Sub Angle: %f"), -SubAngle));
	TargetActor->AddActorWorldRotation(AddedRotation, true);

	PreAngle = CurAngle;
}

void UTCPinchComponent::OnPinchReleased(AActor* TargetActor, const FVector2D Point1, const FVector2D Point2)
{
	 
}

float UTCPinchComponent::ComputeAngleSub(float Angle1, float Angle2)
{
	const float Sub = Angle1 - Angle2;
	if (Sub <= -180.f)
		return Sub + 360.f;
	if (Sub >= 180.f)
		return Sub - 360.f;

	return Sub;
}

void UTCPinchComponent::SetCameraDirection(FRotator& InDir)
{
	CameraRotation = InDir;
}

void UTCPinchComponent::GetLengthAndAngle(float& Length, float& Angle, FVector2D& CenterPos)
{
	Length = (TouchPoints[0] - TouchPoints[1]).Size();
	CenterPos = (TouchPoints[0] + TouchPoints[1]) / 2.f;

	//������Ǽ���Ƕ�
	static const FVector2D ScreenLeftVector(0.f, 1.f);
	static const FVector2D ScreenUpVector(1.f, 0.f);

	const FVector2D PointsVecNormalize = (TouchPoints[0] - TouchPoints[1]).GetSafeNormal();
	const float RotRadians = FMath::Acos(FVector2D::DotProduct(PointsVecNormalize, ScreenUpVector));
	Angle = FMath::RadiansToDegrees(RotRadians);

	//��ȡ��ǰ�Ƕ� -180-180
	if (FVector2D::DotProduct(PointsVecNormalize, ScreenLeftVector) <= 0)
	{
		Angle = 180.f - Angle;
	}
	else
	{
		Angle = Angle - 180.f;
	}
}

