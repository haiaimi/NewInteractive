// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/CustomTouchInterface.h"
#include "TCTapComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WARSIMULATE_API UTCTapComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTCTapComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	int32 GetTouchFingersNum();

	void OnRotateTapPressed(const FVector2D& Point, float DownTime, AActor* TargetActorRef);

	void OnRotateTapUpdated(const FVector2D& Point, float DownTime);

	void OnRotateTapReleased(const FVector2D& Point, float DownTime);

private:
	class AActor* TargetActor;

	FVector2D AnchorPosition;  //按下去的位置，后面的物体旋转就根据这个点

	FRotator InitialRotation;  //按下去Actor初始化的定向

	/**该组件所对应的触控方式*/
	TEnumAsByte<ECustomTouchType::Type> CurTouchType;
};
