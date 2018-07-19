// Fill out your copyright notice in the Description page of Project Settings.

#include "TouchHelper.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "TestProjectHelper.h"
#include "ConvexVolume.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

void TouchHelper::GetTouchInputInfo(class APlayerController* InPlayer, TArray<FVector>& OutTouchPoint)
{
	OutTouchPoint.Empty();
	const FVector* Touches = InPlayer->PlayerInput->Touches;
	
	for (int32 i = 0; i < 11; ++i)
	{
		if (Touches[i].Z != 0)
			OutTouchPoint.Add(Touches[i]);
		else 
			break;
	}
}

bool TouchHelper::IsTouchTypeContained(UObject* TouchComponent, AActor* TargetActor, ECustomTouchType::Type ComponentTouchType)
{
	bool Result = false;
	if (TargetActor->GetClass()->ImplementsInterface(UCustomTouchInterface::StaticClass()))
	{
		ICustomTouchInterface::Execute_CanSuitTargetTouchType(TargetActor, ComponentTouchType, Result);
	}

	return Result;
}

void TouchHelper::GetAllActorsInOrIntersectFrustrum(class APlayerController* InPlayer, FVector2D StartPoint, FVector2D EndPoint, TArray<AActor*>& OutActors, TArray<UClass*> DetectClass)
{
	if ((EndPoint - StartPoint).IsNearlyZero())  //框选距离太短就不进行检测
		return;

	TArray<FVector> WorldPos, WorldDir;
	TArray<FVector2D> ScreenPos;   //屏幕上的位置
	TArray<FPlane,TInlineAllocator<6>> FrustrumPlanes;   //截椎体的平面，不包含两端的
	// 对应屏幕上4个点在世界空间的位置及方向，分别为左上，右上，右下，左下
	ScreenPos.SetNum(4);
	FrustrumPlanes.SetNum(4);
	WorldPos.SetNum(4);
	WorldDir.SetNum(4);
	// 计算出屏幕上的4个点
	const FVector2D TwoPointSub = EndPoint - StartPoint;
	if (TwoPointSub.X > 0 && TwoPointSub.Y > 0)
	{
		ScreenPos[0] = StartPoint;
		ScreenPos[1] = StartPoint + FVector2D(TwoPointSub.X, 0.f);
		ScreenPos[2] = EndPoint;
		ScreenPos[3] = StartPoint + FVector2D(0.f, TwoPointSub.Y);
	}
	else if (TwoPointSub.X > 0 && TwoPointSub.Y < 0)
	{
		ScreenPos[0] = EndPoint - FVector2D(TwoPointSub.X, 0.f);
		ScreenPos[1] = EndPoint;
		ScreenPos[2] = StartPoint - FVector2D(0.f, TwoPointSub.Y);
		ScreenPos[3] = StartPoint;
	}
	else if (TwoPointSub.X < 0 && TwoPointSub.Y > 0)
	{
		ScreenPos[0] = EndPoint - FVector2D(0.f, TwoPointSub.Y);
		ScreenPos[1] = StartPoint;
		ScreenPos[2] = StartPoint + FVector2D(0.f, TwoPointSub.Y);
		ScreenPos[3] = EndPoint;
	}
	else
	{
		ScreenPos[0] = EndPoint;
		ScreenPos[1] = EndPoint - FVector2D(TwoPointSub.X, 0.f);
		ScreenPos[2] = StartPoint;
		ScreenPos[3] = EndPoint - FVector2D(0.f, TwoPointSub.Y);
	}
	// 根据屏幕上的点计算出在世界坐标系中的位置及方向
	for (int32 i = 0; i < 4; i++)
	{
		UGameplayStatics::DeprojectScreenToWorld(InPlayer, ScreenPos[i], WorldPos[i], WorldDir[i]);
	}
	// 根据世界坐标系中的位置及方向求对应的平面
	for (int32 j = 0; j < 4; j++)
	{
		const FVector WorldPointDir = WorldPos[(j + 1) % 4] - WorldPos[j];
		const FVector PlaneNormal = FVector::CrossProduct(WorldDir[j], WorldPointDir);
		FrustrumPlanes[j] = FPlane(WorldPos[j], PlaneNormal.GetSafeNormal());
	}

	FConvexVolume Frustrum(FrustrumPlanes);   //构造检测的截椎体
	
	// 遍历所有已经渲染的actor，判断是否被框选
	for (TActorIterator<AActor> Iter(InPlayer->GetWorld()); Iter; ++Iter)
	{
		bool bDetectedActor = false;
		AActor* CurActor = *Iter;
		for (auto Iter1 : DetectClass)
		{
			if (CurActor->GetClass()->IsChildOf(Iter1))
			{
				bDetectedActor = true;
				break;
			}
		}

		if ((*Iter)->WasRecentlyRendered() && bDetectedActor)
		{
			bool bActorContained = false;
			const FBox ActorBox = CurActor->GetComponentsBoundingBox();
			bool bIntersected = Frustrum.IntersectBox(ActorBox.GetCenter(), ActorBox.GetExtent(), bActorContained);

			if (bIntersected || bActorContained)
			{
				OutActors.Add(CurActor);
			}
		}
	}
}