// Fill out your copyright notice in the Description page of Project Settings.

#include "WarSimulateHelper.h"
#include "Engine/Engine.h"
#include "WarSimulate/WarSimulateProject.h"
#include "Gameplay/MainController.h"
#include "SceneView.h"
#include "Engine/LocalPlayer.h"


void WarSimulateHelper::Debug_ScreenMessage(FString&& InString, FColor FontColor)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FontColor, InString);
}

void WarSimulateHelper::Debug_LogMessage(FString&& InString)
{
	UE_LOG(LogTest, Log, TEXT("%s"), *InString)
}

void WarSimulateHelper::DeprojectScreenToWorld_Cursor(const AMainController* PlayerControl, FVector& OutWorldPos, FVector& OutWorldDir)
{
	FVector2D MousePosition;
	PlayerControl->GetMousePosition(MousePosition.X, MousePosition.Y);    //获取鼠标位置

	FSceneViewProjectionData ProjectionData;     //投影数据
	FViewport* ViewPort = PlayerControl->GetLocalPlayer()->ViewportClient->Viewport;
	PlayerControl->GetLocalPlayer()->GetProjectionData(ViewPort, eSSP_FULL, ProjectionData);      //获取当前的投影数据

	FMatrix const InvViewProjMatrix = ProjectionData.ComputeViewProjectionMatrix().InverseFast();     //获取投影矩阵的逆，这样就可以根据NDC空间得出在世界坐标系中的位置
	FSceneView::DeprojectScreenToWorld(MousePosition, ProjectionData.GetConstrainedViewRect(), InvViewProjMatrix, OutWorldPos, OutWorldDir);   //其流程就是: 屏幕坐标-->NDC坐标-->世界坐标
}

void WarSimulateHelper::DeprojectScreenToWorld_SpecifyPoint(const AMainController* PlayerControl, FVector2D ScreenPos, FVector& OutWorldPos, FVector& OutWorldDir)
{
	FVector2D ScreenSize;
	PlayerControl->GetLocalPlayer()->ViewportClient->GetViewportSize(ScreenSize);   //获取屏幕尺寸

	FSceneViewProjectionData ProjectionData;     //投影数据
	FViewport* ViewPort = PlayerControl->GetLocalPlayer()->ViewportClient->Viewport;
	PlayerControl->GetLocalPlayer()->GetProjectionData(ViewPort, eSSP_FULL, ProjectionData);      //获取当前的投影数据

	FMatrix const InvViewProjMatrix = ProjectionData.ComputeViewProjectionMatrix().InverseFast();     //获取投影矩阵的逆，这样就可以根据NDC空间得出在世界坐标系中的位置
	FSceneView::DeprojectScreenToWorld(ScreenSize * ScreenPos, ProjectionData.GetConstrainedViewRect(), InvViewProjMatrix, OutWorldPos, OutWorldDir);
}
