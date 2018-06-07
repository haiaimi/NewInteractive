// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryActor.generated.h"

/**
 * 该类是仓库中物体的基类，只包含基本功能，负责基本的场景显示，其他详细逻辑在子类中
 */
UCLASS()
class TESTPROJECT_API AInventoryActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInventoryActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents()override;

public:
	UFUNCTION()
	virtual void BeginCursorOver(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	virtual void EndCursorOver(UPrimitiveComponent* TouchedComponent);

	/**跟随鼠标移动*/
	void StartMoveWithCursor(class AMainController* Owner, const FVector Offset, const FPlane MovePalne);

	void StopMoveWithCursor();

public:
	UPROPERTY()
	class UStaticMeshComponent* ActorMesh;

	/**基础场景，为场景的根*/
	UPROPERTY()
	class USceneComponent* BaseScene;

private:
	FVector RelativeLoc;

	FVector RelativeRot;

	/**控制该物体的Controller*/
	class AMainController* OwnerController;

	/**物体移动的平面，因为要根据鼠标位置移动，所以目前只使用二维平面的移动*/
	FPlane MovePlane;

	FVector Offset;

	uint32 MoveWithCursor : 1;
};
