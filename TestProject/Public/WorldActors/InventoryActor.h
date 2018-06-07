// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryActor.generated.h"

/**
 * �����ǲֿ�������Ļ��ֻ࣬�����������ܣ���������ĳ�����ʾ��������ϸ�߼���������
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

	/**��������ƶ�*/
	void StartMoveWithCursor(class AMainController* Owner, const FVector Offset, const FPlane MovePalne);

	void StopMoveWithCursor();

public:
	UPROPERTY()
	class UStaticMeshComponent* ActorMesh;

	/**����������Ϊ�����ĸ�*/
	UPROPERTY()
	class USceneComponent* BaseScene;

private:
	FVector RelativeLoc;

	FVector RelativeRot;

	/**���Ƹ������Controller*/
	class AMainController* OwnerController;

	/**�����ƶ���ƽ�棬��ΪҪ�������λ���ƶ�������Ŀǰֻʹ�ö�άƽ����ƶ�*/
	FPlane MovePlane;

	FVector Offset;

	uint32 MoveWithCursor : 1;
};
