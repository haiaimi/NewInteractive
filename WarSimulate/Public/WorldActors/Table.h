// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Table.generated.h"

UCLASS()
class WARSIMULATE_API ATable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents()override;

public:
	/**切换桌面网格的材质*/
	void ToggleMaterial(UMaterialInterface* InMaterial);

	UFUNCTION()
	void BeginCursorOver(UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	void EndCursorOver(UPrimitiveComponent* TouchedComponent);

public:
	UPROPERTY()
	class USceneComponent* BaseSceneComponent;
	/**桌面网格组建*/
	UPROPERTY()
	class UStaticMeshComponent* TableMeshComponent;

	/**桌面静态网格*/
	UPROPERTY()
	class UStaticMesh* TableMesh;
	
	/**桌面材质*/
	UPROPERTY()
	class UMaterialInterface* TableMaterial;
};
