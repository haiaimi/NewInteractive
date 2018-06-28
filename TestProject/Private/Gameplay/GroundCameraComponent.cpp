// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/GroundCameraComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "TestProjectHelper.h"

static const float GlobalDetectTime = 0.1f;

UGroundCameraComponent::UGroundCameraComponent()
{
	LookRotation = FRotator(-70.f, 0.f, 0.f);      //默认角度
	LastUpdateTime = 0.f;
	bDecelerate = false;
}

void UGroundCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	static float CurSpeedScale = 1.f;
	APlayerController* Controller = GetPlayerController();
	if (Controller)
	{
		if (bDecelerate)
		{
			float NewSpeedScale = FMath::FInterpTo(CurSpeedScale, 0.f, DeltaTime, 3.f);
			GetOwner()->SetActorLocation(GetOwner()->GetActorLocation() + DecelerateSpeed * NewSpeedScale * DeltaTime);
			CurSpeedScale = NewSpeedScale;

			if (NewSpeedScale <= 0.01f)
			{
				bDecelerate = false;
			}
		}
		else
			CurSpeedScale = 1.f;

		DesiredView.Rotation = LookRotation;   //
		DesiredView.Location = Controller->GetFocalLocation();
	}
}

void UGroundCameraComponent::MoveRight(float Val)
{

}

void UGroundCameraComponent::MoveForward(float Val)
{

}

APlayerController* UGroundCameraComponent::GetPlayerController()
{
	APawn* Owner = Cast<APawn>(GetOwner());
	APlayerController* Controller = nullptr;
	if (Owner)
	{
		Controller = Cast<APlayerController>(Owner->Controller);
	}

	return Controller;
}

void UGroundCameraComponent::StartSwipe(const FVector2D& InPoint, float DownTime)
{
	bDecelerate = false;
	APlayerController* Controller = GetPlayerController();
	if (Controller)
	{
		FHitResult HitResult;
		if (Controller->GetHitResultAtScreenPosition(InPoint, ECollisionChannel::ECC_WorldDynamic, true, HitResult))
		{
			StartSwipePos = HitResult.ImpactPoint;   
		}
	}
}

void UGroundCameraComponent::UpdateSwipe(const FVector2D& InPoint, float DownTime)
{
	static float PreDownTime = 0.f;
	static float DetectTime = 0.f;
	//static TArray<FVector> TracePoints;     // 用来记录触摸经过的轨迹
	if (PreDownTime > DownTime)   //此状态就是已经重新开始更新
	{
		PreDownTime = 0.f;
		DetectTime = 0.f;
	}

	APlayerController* Controller = GetPlayerController();
	AMainController* MyController = Cast<AMainController>(Controller);
	LastUpdateTime = DownTime;

	if (Controller && MyController && MyController->CanDragLanscape())
	{
		FHitResult HitResult;
		if (Controller->GetHitResultAtScreenPosition(InPoint, ECollisionChannel::ECC_WorldDynamic, true, HitResult))
		{
			FVector DeltaPos = StartSwipePos - HitResult.ImpactPoint;
			DeltaPos.Z = 0.f;        //忽略Z分量

			if (!DeltaPos.IsNearlyZero())
			{
				FVector CurLocation = GetOwner()->GetActorLocation();
				GetOwner()->SetActorLocation(CurLocation + DeltaPos);
			}

			if (DetectTime < GlobalDetectTime)
			{
				DetectTime += (DownTime - PreDownTime);      //
				TracePoints.Add(DeltaPos);
			}
			else
			{
				// 如果超过要求的记录时间就增加一个的同时删除一个点
				TracePoints.Add(DeltaPos);
				TracePoints.RemoveAt(0);
			}
			
			
			//DecelerateSpeed= DeltaPos / (DownTime - PreDownTime);     //计算最终移动速度
			PreDownTime = DownTime;
		}
	}
}

void UGroundCameraComponent::EndSwipe(const FVector2D& InPoint, float DownTime)
{
	//StartSwipePos = GetOwner()->GetActorLocation();
	bDecelerate = true;
	FVector DeclerateVec;
	for (FVector& It : TracePoints)
	{
		DeclerateVec += It;
	}

	if (DownTime < GlobalDetectTime)
		DecelerateSpeed = DeclerateVec / DownTime;
	if (DownTime >= GlobalDetectTime)
		DecelerateSpeed = DeclerateVec / GlobalDetectTime;

	TracePoints.Empty();   //清空数组内容
}
