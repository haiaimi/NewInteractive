// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/CustomTouchInterface.h"
#include "Locker.generated.h"

/**
 *  储物柜类，用于模拟储物柜，管理其中的物体 
 */
UCLASS()
class TESTPROJECT_API ALocker : public AActor, public ICustomTouchInterface
{
	GENERATED_BODY()
		
	/// Begin CustomTouchInterface
	virtual void GetTouchTypes_Implementation(TArray<TEnumAsByte<ECustomTouchType::Type>>& OutTypes) override;

	virtual void AddTouchTypes_Implementation(ECustomTouchType::Type InType) override;

	virtual void RemoveTouchTypes_Implementation(ECustomTouchType::Type InType)override;

	virtual void CanSuitTargetTouchType_Implementation(ECustomTouchType::Type TargetTouchType, bool& Out)override;

	virtual void InterfaceFunction(ECustomTouchType::Type InType)override {};
	/// End CustomTouchInterface

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

	/**控制储物柜的伸收*/
	void Switch();

	FVector GetRelativeLocationToPawn_Hide() { return RelativeToPawn_Hide; }

	void UpdateRelativePosToPawn(class FViewport* InViewport, uint32 i);

	/**设置Locker可见性*/
	void SetVisibility(bool bVisible);

	UFUNCTION()
	void BeginMove(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);

	void UpdateMove();

	UFUNCTION()
	void EndMove(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);

	void StartOpenLocker(const FVector2D& Point, float DownTime);

	void UpdateOpenLocker(const FVector2D& Point, float DownTime);

	void EndOpenLocker(const FVector2D& Point, float DownTime);

	/**获取在Locker外的InventoryActor*/
	TArray<class AInventoryActor*>& GetInventoryActorOutside();

private:
	FVector GetRelativeLocationToPawn();

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

	class AMainController* OwnerController;

	uint8 bInMove : 1;

	uint8 bInShow : 1;

	uint8 bCanUpdate : 1;

	/**检测轨迹的开始，用于检测是否触发Locker*/
	uint8 bStartTraceLine : 1;

	/**检测线上的点*/
	FVector2D StartPoint;

	/**Locker正常显示时相对于Pawn的位置*/
	FVector RelativeToPawn_Show;

	/**Locker隐藏时相对于Pawn的位置*/
	FVector RelativeToPawn_Hide;

	/**触发Touch事件后Locker中心离触摸点的距离*/
	FVector TouchOffset;      

	/**当前触摸的Index，为了从Controller获取触摸点位置*/
	ETouchIndex::Type FingerIndex;

public:
	float LockerLength;

	float LockerWidth;

	/***/
	int32 LockerCapacity;
};
