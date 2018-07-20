// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CustomTouchInterface.h"
#include "TCDragSwipeComponent.generated.h"

/**
 * 单指拖拽的功能 
 */

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESTPROJECT_API UTCDragSwipeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTCDragSwipeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OnDragPressed(const FVector2D& Point, FVector& LookDir, class AActor* InTargetActor, TArray<AActor*>& InActors);

	void OnDragUpdate(const FVector2D& Point, FVector& LookDir, TArray<AActor*>& InActors);

	void OnDragReleased(const FVector2D& Point);

	/**检测触摸点是否存在合适的Actor*/
	void DetectActorOnPoint(const FVector2D& Point, FVector& LookDir);

	/**查看是否在拖拽Actor*/
	bool IsDragActor();

public:
	class AActor* TargetActor;

private:
	ECustomTouchType::Type CurTouchType;

	/**触摸点到Actor中心的偏移量*/
	FVector TouchOffset;

	/**目标Acto移动的平面，现阶段只进行二维移动*/
	FPlane MovePlane;

	/**多目标移动*/
	uint8 bMultiMove : 1;
};
