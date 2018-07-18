// Fill out your copyright notice in the Description page of Project Settings.

#include "TCDragSwipeComponent.h"
#include "GameFramework/PlayerController.h"
#include "TouchHelper.h"
#include "Engine/World.h"


// Sets default values for this component's properties
UTCDragSwipeComponent::UTCDragSwipeComponent() :
	TargetActor(nullptr),
	CurTouchType(ECustomTouchType::Drag_1P)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UTCDragSwipeComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UTCDragSwipeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTCDragSwipeComponent::OnDragPressed(const FVector2D& Point, FVector& LookDir)
{ 
	DetectActorOnPoint(Point, LookDir);
	StartPoint = Point;
}

void UTCDragSwipeComponent::OnDragUpdate(const FVector2D& Point, FVector& LookDir)
{
	APlayerController* MyController = Cast<APlayerController>(GetOwner());
	if (TargetActor)
	{
		if (MyController)
		{
			FVector WorldPos, WorldDir;
			MyController->DeprojectScreenPositionToWorld(Point.X, Point.Y, WorldPos, WorldDir);

			FVector ActorNewPos = FMath::LinePlaneIntersection(WorldPos, WorldPos + 1000.f*WorldDir, MovePlane);
			TargetActor->SetActorLocation(ActorNewPos + TouchOffset);     //更新Actor的位置
		}
	}
	else
	{
		//不存在目标Actor则继续检测
		DetectActorOnPoint(Point, LookDir);
	}
}

void UTCDragSwipeComponent::OnDragReleased(const FVector2D& Point)
{
	TArray<AActor> temp;
	TouchHelper::GetAllActorsInOrIntersectFrustrum(GetWorld()->GetFirstPlayerController(), StartPoint, Point, temp);
	TargetActor = nullptr;
}

void UTCDragSwipeComponent::DetectActorOnPoint(const FVector2D& Point, FVector& LookDir)
{
	if (APlayerController* MyController = Cast<APlayerController>(GetOwner()))
	{
		FHitResult Result;
		MyController->GetHitResultAtScreenPosition(Point, ECollisionChannel::ECC_WorldDynamic, false, Result);

		if (Result.GetActor())
		{
			if (TouchHelper::IsTouchTypeContained(this, Result.GetActor(), ECustomTouchType::Drag_1P))
			{
				TargetActor = Result.GetActor();
				TouchOffset = TargetActor->GetActorLocation() - Result.ImpactPoint;   //鼠标指针相对于物体的位置
				MovePlane = FPlane(Result.ImpactPoint, -LookDir);   //获取鼠标与物体撞击点的平面
			}
		}
	}
}