// Fill out your copyright notice in the Description page of Project Settings.

#include "TCTapComponent.h"
#include "TouchHelper.h"
#include "GameFramework/PlayerController.h"


// Sets default values for this component's properties
UTCTapComponent::UTCTapComponent()
	:TargetActor(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UTCTapComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UTCTapComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

int32 UTCTapComponent::GetTouchFingersNum()
{
	if (APlayerController* MyController = Cast<APlayerController>(GetOwner()))
	{
		TArray<FVector> TouchPoints;
		TouchHelper::GetTouchInputInfo(MyController, TouchPoints);

		return TouchPoints.Num();
	}

	return 0;
}

void UTCTapComponent::OnRotateTapPressed(const FVector2D& Point, float DownTime)
{
	if (APlayerController* MyController = Cast<APlayerController>(GetOwner()))
	{
		FHitResult Result;
		MyController->GetHitResultAtScreenPosition(Point, ECollisionChannel::ECC_WorldDynamic, false, Result);
		
		if (Result.GetActor())
		{
			TargetActor = Result.GetActor();
			AnchorPosition = Point;
			InitialRotation = TargetActor->GetActorRotation();
		}
	}
}

void UTCTapComponent::OnRotateTapUpdated(const FVector2D& Point, float DownTime)
{
	if (TargetActor)
	{
		FVector2D Offset = Point - AnchorPosition;
		FRotator Rotation(0.f, Offset.Y, Offset.X);
		TargetActor->SetActorRotation(Rotation);
	}
}		

void UTCTapComponent::OnRotateTapReleased(const FVector2D& Point, float DownTime)
{

}

