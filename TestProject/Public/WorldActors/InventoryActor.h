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

private:
	UPROPERTY()
	class UStaticMeshComponent* ActorMesh;

	/**����������Ϊ�����ĸ�*/
	UPROPERTY()
	class USceneComponent* BaseScene;

	FVector RelativeLoc;

	FVector RelativeRot;
};
