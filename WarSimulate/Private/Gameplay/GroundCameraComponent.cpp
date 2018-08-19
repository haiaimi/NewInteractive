// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/GroundCameraComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "OriginHelper.h"
#include "CustomTouchInput.h"

static const float GlobalDetectTime = 0.1f;
static const float ZoomHeight = 20000.f;      //滚动的高度
static const float BaseHeight = 20000.f;
static const FRotator CamDefaultRotation = FRotator(-70.f, 0.f, 0.f);    //默认摄像机角度

UGroundCameraComponent::UGroundCameraComponent():
	bInBlur(false)
{
	LookRotation = CamDefaultRotation;
	LastUpdateTime = 0.f;
	bDecelerate = false;
	InitialPinchAlpha = 0.f;
	ZoomAlpha = 0.5f;
	bInPinch = false;

	PostProcessSettings.bOverride_DepthOfFieldMethod = false;
	PostProcessSettings.DepthOfFieldMethod = EDepthOfFieldMethod::DOFM_Gaussian;
	PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
	PostProcessSettings.DepthOfFieldFocalDistance = 1.f;
	PostProcessSettings.bOverride_DepthOfFieldFocalRegion = true;
	PostProcessSettings.DepthOfFieldFocalRegion = 280.f;
	PostProcessSettings.bOverride_DepthOfFieldNearBlurSize = true;
	PostProcessSettings.DepthOfFieldNearBlurSize = 1.f;
	PostProcessSettings.bOverride_DepthOfFieldFarBlurSize = true;
	PostProcessSettings.DepthOfFieldFarBlurSize = 0.f;
}

void UGroundCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	static float CurSpeedScale = 1.f;
	const APlayerController* Controller = GetPlayerController();

	if (Controller)
	{
		FVector PawnLocation = GetOwner()->GetActorLocation();
		PawnLocation.Z = BaseHeight;
		if (bDecelerate)
		{
			float NewSpeedScale = FMath::FInterpTo(CurSpeedScale, 0.f, DeltaTime, 3.f);
			PawnLocation += DecelerateSpeed * NewSpeedScale * DeltaTime;
			CurSpeedScale = NewSpeedScale;

			if (NewSpeedScale <= 0.01f)
			{
				bDecelerate = false;
			}
		}
		else
			CurSpeedScale = 1.f;

		PawnLocation += FVector::UpVector * ZoomHeight * ZoomAlpha;
		GetOwner()->SetActorLocation(PawnLocation);
		DesiredView.Rotation = LookRotation;   //
		DesiredView.Location = Controller->GetFocalLocation();
		
		if (bInBlur && PostProcessSettings.DepthOfFieldFarBlurSize != 32.f)
		{
			const float FarBlurSize = FMath::FInterpConstantTo(PostProcessSettings.DepthOfFieldFarBlurSize, 32.f, DeltaTime, 32.f);
			PostProcessSettings.DepthOfFieldFarBlurSize = FarBlurSize;
		}
		else if (!bInBlur && PostProcessSettings.DepthOfFieldFarBlurSize != 0.f)
		{
			const float FarBlurSize = FMath::FInterpConstantTo(PostProcessSettings.DepthOfFieldFarBlurSize, 0.f, DeltaTime, 40.f);
			PostProcessSettings.DepthOfFieldFarBlurSize = FarBlurSize;
			if (PostProcessSettings.DepthOfFieldFarBlurSize == 0.f)
				PostProcessSettings.bOverride_DepthOfFieldMethod = false;  //关闭模糊

		} 

		DesiredView.PostProcessSettings = PostProcessSettings;
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

	if (Controller && MyController && MyController->CanDragLanscape() &&!bInPinch)
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
			
			PreDownTime = DownTime;
		}
	}

	if (bInPinch)
	{
		if (Controller)
		{
			FHitResult HitResult;
			if (Controller->GetHitResultAtScreenPosition(InPoint, ECollisionChannel::ECC_WorldDynamic, true, HitResult))
			{
				StartSwipePos = HitResult.ImpactPoint;
			}
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

void UGroundCameraComponent::OnPinchStart(const FVector2D& InPoint1, const FVector2D& InPoint2, float DownTime)
{
	InitialPinchAlpha = ZoomAlpha;
	bInPinch = true;
}

void UGroundCameraComponent::OnPinchUpdate(UCustomTouchInput* InputHandle, const FVector2D& InPoint1, const FVector2D& InPoint2, float DownTime)
{
	FVector2D* const Anchors = InputHandle->GetTouchAnchors();
	const float AnchorDistance = (Anchors[0] - Anchors[1]).Size();
	const float CurrentDistance = (InPoint2 - InPoint1).Size();
	const float PinchDelta = AnchorDistance - CurrentDistance;
	
	ZoomAlpha = FMath::Clamp(InitialPinchAlpha + PinchDelta * 0.002f, 0.f, 1.f);
}

void UGroundCameraComponent::OnPinchEnd(const FVector2D& InPoint1, const FVector2D& InPoint2, float DownTime)
{
	bInPinch = false;
}

void UGroundCameraComponent::BlurMode(bool bBlur)
{
	bInBlur = bBlur;
	if (bBlur)
	{
		PostProcessSettings.bOverride_DepthOfFieldMethod = true;
	}
	else
	{
		PostProcessSettings.DepthOfFieldFarBlurSize = 15.f;
	}
}
