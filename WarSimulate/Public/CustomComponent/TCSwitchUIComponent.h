// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TCSwitchUIComponent.generated.h"

DECLARE_DELEGATE(FTriggerEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WARSIMULATE_API UTCSwitchUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTCSwitchUIComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OnFivePointsPressed(const TArray<FVector2D>& Points, float DownTime);

	/**在更新的时候进行检测，以提高灵敏度*/
	void OnFivePointsUpdate(const TArray<FVector2D>& Points, float DownTime);

	void OnFivePointsReleased(const TArray<FVector2D>& Points, float DownTime);
	
	template<class T>
	static float ComputePointsPerimeter(const TArray<T>& InPoints)
	{
		float AnchorPerimeter = 0.f;      //5个锚点的周长
		float MinDistance = MAX_int32;
		TArray<int32> ComparedIndex; //已比较过的点下标数组

		T Temp = InPoints[0];
		ComparedIndex.Add(0);

		for (int32 i = 0; i < InPoints.Num() - 1; ++i)
		{
			int32 MinPointIndex = 0;

			for (int32 j = 0; j < InPoints.Num(); ++j)
			{
				if (ComparedIndex.Find(j) == INDEX_NONE)  //当前目录不存在数组中
				{
					float TempDistance = (Temp - InPoints[j]).Size();
					if (MinDistance > TempDistance)
					{
						MinDistance = TempDistance;
						MinPointIndex = j;
					}
				}
			}

			AnchorPerimeter += MinDistance;
			Temp = InPoints[MinPointIndex];
			ComparedIndex.Add(MinPointIndex);

			if (i == (InPoints.Num() - 1))
			{
				AnchorPerimeter += (Temp - InPoints[0]).Size();
			}
			MinDistance = MAX_int32;
		}

		return AnchorPerimeter;
	}

public:
	/**Released时触发的事件*/
	FTriggerEvent ReleasedEventOpen;
	FTriggerEvent ReleasedEventClose;

private:
	/**按下时的5个点*/
	TArray<FVector2D> AnchorPoints;    

	/**按下时5个点的周长*/
	float AnchorPerimeter;

	float PressedTime;
};