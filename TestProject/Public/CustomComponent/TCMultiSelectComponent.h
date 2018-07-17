// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TCMultiSelectComponent.generated.h"

/**
  * 该组件是多选触摸组件，类似于Windows桌面上的鼠标多选功能，这里是使用触摸
  */

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESTPROJECT_API UTCMultiSelectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTCMultiSelectComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OnMultiSelectPressed(const FVector2D& Point, float DownTime);

	void OnMultiSelectUpdate(const FVector2D& Point, float DownTime);

	void OnMultiSelectReleased(const FVector2D& Point, float DownTime);
	
};
