// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/GroundSpectatorPawn.h"
#include "Gameplay/GroundCameraComponent.h"
#include "Gameplay/GroundSpectatorPawnMovement.h"
#include "Components/SphereComponent.h"
#include "Engine/CollisionProfile.h"


AGroundSpectatorPawn::AGroundSpectatorPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UGroundSpectatorPawnMovement>(Super::MovementComponentName))       //使用自定义的MoveComponent
{
	GetCollisionComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);     //忽略所有碰撞
	SpectatorCamera = ObjectInitializer.CreateDefaultSubobject<UGroundCameraComponent>(this, TEXT("SpectatorCamera"));
}

void AGroundSpectatorPawn::MoveRight(float Val)
{

}

void AGroundSpectatorPawn::MoveForward(float Val)
{

}

void AGroundSpectatorPawn::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{

}

