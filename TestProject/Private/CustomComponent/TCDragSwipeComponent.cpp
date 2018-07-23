// Fill out your copyright notice in the Description page of Project Settings.

#include "TCDragSwipeComponent.h"
#include "GameFramework/PlayerController.h"
#include "TouchHelper.h"
#include "Engine/World.h"
#include "InventoryActor.h"
#include "TestProjectHelper.h"
#include "Components/StaticMeshComponent.h"


// Sets default values for this component's properties
UTCDragSwipeComponent::UTCDragSwipeComponent() :
	TargetActor(nullptr),
	CurTouchType(ECustomTouchType::Drag_1P),
	bMultiMove(false)
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

void UTCDragSwipeComponent::OnDragPressed(const FVector2D& Point, FVector& LookDir, class AActor* InTargetActor, TArray<AActor*>& InActors)
{ 
	if (InTargetActor)
		TargetActor = InTargetActor;

	DetectActorOnPoint(Point, LookDir);

	if (TargetActor == nullptr || InActors.Find(TargetActor) == INDEX_NONE)    //当前指向的Actor不在多选Actor数组中
	{
		for (TArray<AActor*>::TIterator Iter(InActors); Iter; ++Iter)     //这里使用迭代器
		{
			UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>((*Iter)->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			if (MeshComponent)
				MeshComponent->SetRenderCustomDepth(false);     //停止高亮渲染
		}
	}
	else if (InActors.Num() > 1)
		bMultiMove = true;
}

void UTCDragSwipeComponent::OnDragUpdate(const FVector2D& Point, FVector& LookDir, TArray<AActor*>& InActors)
{
	APlayerController* MyController = Cast<APlayerController>(GetOwner());
	if (TargetActor && TouchHelper::IsTouchTypeContained(this, TargetActor, ECustomTouchType::Drag_1P))
	{
		FVector RelativeLoc;     //TargetActor当前位置相对于上一次的位置
		if (MyController)
		{
			FVector WorldPos, WorldDir;
			MyController->DeprojectScreenPositionToWorld(Point.X, Point.Y, WorldPos, WorldDir);

			FVector ActorNewPos = FMath::LinePlaneIntersection(WorldPos, WorldPos + 1000.f*WorldDir, MovePlane);
			RelativeLoc = ActorNewPos + TouchOffset - TargetActor->GetActorLocation();
			if (!bMultiMove)
				TargetActor->SetActorLocation(ActorNewPos + TouchOffset);     //更新Actor的位置
			else
			{
				for (TArray<AActor*>::TIterator Iter(InActors); Iter; ++Iter)     //这里使用迭代器
				{
					if (TouchHelper::IsTouchTypeContained(this, *Iter, CurTouchType))
						(*Iter)->SetActorLocation((*Iter)->GetActorLocation() + RelativeLoc);    //更新位置
				}
			}
		}
	}
}

void UTCDragSwipeComponent::OnDragReleased(const FVector2D& Point)
{
	TargetActor = nullptr;
	bMultiMove = false;
}

void UTCDragSwipeComponent::DetectActorOnPoint(const FVector2D& Point, FVector& LookDir)
{
	if (APlayerController* MyController = Cast<APlayerController>(GetOwner()))
	{
		FHitResult Result;
		MyController->GetHitResultAtScreenPosition(Point, ECollisionChannel::ECC_WorldDynamic, false, Result);

		if (Result.GetActor())
		{
			TargetActor = Result.GetActor();
			TouchOffset = TargetActor->GetActorLocation() - Result.ImpactPoint;   //鼠标指针相对于物体的位置
			MovePlane = FPlane(Result.ImpactPoint, -LookDir);   //获取鼠标与物体撞击点的平面
		}
	}
}

bool UTCDragSwipeComponent::IsDragActor()
{
	return TargetActor != nullptr;
}
