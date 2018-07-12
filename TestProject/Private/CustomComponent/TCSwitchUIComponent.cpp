// Fill out your copyright notice in the Description page of Project Settings.

#include "TCSwitchUIComponent.h"
#include "TestProjectHelper.h"

static const float PressToReleaseInterval = 1.f;     //按下松开的最大间隔时间为1秒

// Sets default values for this component's properties
UTCSwitchUIComponent::UTCSwitchUIComponent():
	PressedTime(0.f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UTCSwitchUIComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UTCSwitchUIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTCSwitchUIComponent::OnFivePointsPressed(const TArray<FVector2D>& Points, float DownTime)
{
	PressedTime = DownTime;
	AnchorPoints = Points;

	AnchorPerimeter = UTCSwitchUIComponent::ComputePointsPerimeter(AnchorPoints);
}

void UTCSwitchUIComponent::OnFivePointsUpdate(const TArray<FVector2D>& Points, float DownTime)
{
	float CurrentPerimeter = UTCSwitchUIComponent::ComputePointsPerimeter(Points);
	float CurrentScale = CurrentPerimeter / AnchorPerimeter;     //计算当前点周长和按下时周长的比例

	if (CurrentScale < 0.8f)
		ReleasedEventOpen.ExecuteIfBound();
	else if (CurrentScale > 1.3f)
		ReleasedEventClose.ExecuteIfBound();
}

void UTCSwitchUIComponent::OnFivePointsReleased(const TArray<FVector2D>& Points, float DownTime)
{
	/**
	 * 原本是使用从5个点中取一个中心锚点来判断，但是总会存在判断错误情况，
	 * 所以使用计算周长的方法，比较按下时5个点的形成的多边形的周长和松开时5个点形成的多边形的周长
	 */

	///下面的是根据中心点测试，效果不太行
	//FVector2D AnchorCenter;
	//float MaxDistance = 0.f;    //五指中两点的最大距离
	//for (auto& It1 : AnchorPoints)
	//	for (auto& It2 : AnchorPoints)
	//	{
	//		float TempDistance = (It2 - It1).Size();
	//		if (TempDistance > MaxDistance)
	//		{
	//			MaxDistance = TempDistance;
	//			AnchorCenter = (It2 - It1) / 2;
	//		}
	//	}

	////AnchorCenter /= Points.Num();

	//int32 OutPointNum = 0, InPointNum = 0;
	//if (Points.Num() <= 5)
	//	for (int32 i = 0; i < Points.Num(); ++i)
	//	{
	//		if ((Points[i] - AnchorCenter).Size() >(AnchorPoints[i] - AnchorCenter).Size())
	//			OutPointNum++;
	//		else
	//			InPointNum++;
	//	}

	//if (OutPointNum >= 3)
	//	ReleasedEventOpen.ExecuteIfBound();
	//if (InPointNum == Points.Num())
	//	ReleasedEventClose.ExecuteIfBound();

	///下面是根据周长
	float ReleasedPointsPerimeter = UTCSwitchUIComponent::ComputePointsPerimeter(Points);

	if (DownTime - PressedTime < PressToReleaseInterval)
	{
		if (ReleasedPointsPerimeter < AnchorPerimeter)
			ReleasedEventOpen.ExecuteIfBound();
		else
			ReleasedEventClose.ExecuteIfBound();
	}
}

