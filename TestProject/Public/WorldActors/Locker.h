// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Locker.generated.h"

/**
 *  储物柜类，用于模拟储物柜，管理其中的物体 
 */
UCLASS()
class TESTPROJECT_API ALocker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALocker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	/** 根据鼠标结束拖拽最后的位置来放置物体*/
	void AddInventoryThing(class AInventoryActor* AddedActor, FVector CursorLastPoint);

	void RemoveInventoryThing(class AInventoryActor* RemovedActor);

	/**给物体投射光线*/
	void CastLight(class AInventoryActor* CastedActor);

	void StopCastLight();

private:
	/**储物柜中存放的东西*/
	UPROPERTY()
	TArray<class AInventoryActor*> LockerContent;
	
	///基本场景
	UPROPERTY()
	class UMaterialInterface* LockerMaterial;

	UPROPERTY()
	class UStaticMesh* LockerMesh;

	UPROPERTY()
	class UStaticMeshComponent* LockerMeshComponent;

	UPROPERTY()
	class USceneComponent* BaseScene;
	
	/**聚光灯组件，用于照亮物体*/
	UPROPERTY()
	class USpotLightComponent* SpotLight;

public:
	float LockerLength;

	float LockerWidth;

	/***/
	int32 LockerCapacity;
};
