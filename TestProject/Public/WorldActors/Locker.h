// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Locker.generated.h"

/**
 *  ������࣬����ģ�ⴢ��񣬹������е����� 
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

private:
	/**������д�ŵĶ���*/
	UPROPERTY()
	TArray<class AInventoryActor*> LockerContent;
	
	///��������
	UPROPERTY()
	class UMaterialInterface* LockerMaterial;

	UPROPERTY()
	class UStaticMesh* LockerMesh;

	UPROPERTY()
	class UStaticMeshComponent* LockerMeshComponent;

	UPROPERTY()
	class USceneComponent* BaseScene;
	
	/**�۹�������������������*/
	UPROPERTY()
	class USpotLightComponent* SpotLight;
};