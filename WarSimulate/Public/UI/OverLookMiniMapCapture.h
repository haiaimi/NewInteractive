// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/SceneCapture2D.h"
#include "OverLookMiniMapCapture.generated.h"

/**
 *  俯视摄像机，用于监视飞机飞行状态
 */
UCLASS()
class WARSIMULATE_API AOverLookMiniMapCapture : public ASceneCapture2D
{
	GENERATED_BODY()
	
public:
	AOverLookMiniMapCapture();

	virtual void Tick(float DeltaTime)override;

	virtual void BeginPlay()override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MiniMap, meta = (ClampMin = "0", ClampMax = "1024"))
	int32 MiniMapWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MiniMap, meta = (ClampMin = "0", ClampMax = "1024"))
	int32 MiniMapHeight;

	/**捕获摄像机的FOV*/
	float CameraFOV;

	/**摄像机位置*/
	FVector CameraLoc;

	class UTextureRenderTarget2D* MiniMapRenderTarget;

	/**镜头旋转的目标角度*/
	FRotator DstRotator;

	/**该值会用于HUD中人物方向*/
	float RotatorYaw;

	float CachedRotatorYaw;

	uint32 bUpdateRot : 1;

public:
	/**设置捕获相机的位置*/
	void SetSceneCaptureLocation(FVector Loc);
};
