// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/GroundCameraComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"


UGroundCameraComponent::UGroundCameraComponent()
{
	LookRotation = FRotator(-70.f, 0.f, 0.f);      //д╛хо╫г╤х
}

void UGroundCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	APlayerController* Controller = GetPlayerController();
	if (Controller)
	{
		DesiredView.Rotation = LookRotation;   //
		DesiredView.Location = Controller->GetFocalLocation();
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
