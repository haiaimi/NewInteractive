// Fill out your copyright notice in the Description page of Project Settings.

#include "OverLookMiniMapCapture.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"


AOverLookMiniMapCapture::AOverLookMiniMapCapture()
{
	GetCaptureComponent2D()->bCaptureEveryFrame = false;       //不进行每帧更新，只有移动或加载的时候进行渲染
	GetCaptureComponent2D()->bCaptureOnMovement = true;

	PrimaryActorTick.bCanEverTick = true;
	CameraFOV = 60.f;
	MiniMapHeight = 256;
	MiniMapWidth = 256;
	bUpdateRot = false;
}

void AOverLookMiniMapCapture::BeginPlay()
{
	Super::BeginPlay();

	MiniMapRenderTarget = NewObject<UTextureRenderTarget2D>();
	MiniMapRenderTarget->InitAutoFormat(MiniMapWidth, MiniMapHeight);

	GetCaptureComponent2D()->TextureTarget = MiniMapRenderTarget;
	GetCaptureComponent2D()->FOVAngle = CameraFOV;
	RootComponent->SetWorldRotation(FRotator(-90.f, 0.f, 0.f));
}

void AOverLookMiniMapCapture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOverLookMiniMapCapture::SetSceneCaptureLocation(FVector Loc)
{
	RootComponent->SetWorldLocation(Loc + FVector(0.f, 0.f, 10000.f));
}