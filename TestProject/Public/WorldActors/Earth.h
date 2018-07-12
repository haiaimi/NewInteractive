// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldActors/InventoryActor.h"
#include "Earth.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJECT_API AEarth : public AInventoryActor
{
	GENERATED_BODY()

public:
	AEarth();

protected:
	virtual void BeginPlay()override;

public:
	virtual void Tick(float DeltaTime)override;

	virtual void PostInitializeComponents()override;

	// CustomTouchInterface
	virtual void GetTouchTypes_Implementation(TArray<TEnumAsByte<ECustomTouchType::Type>>& OutTypes) override;

	virtual void AddTouchTypes_Implementation(ECustomTouchType::Type InType) override;

	virtual void CanSuitTargetTouchType_Implementation(TEnumAsByte<ECustomTouchType::Type>& TargetTouchType, bool& Out) override;

	virtual void InterfaceFunction_Implementation(ECustomTouchType::Type InType)override {};

public:
	UFUNCTION()
	virtual void BeginCursorOver(UPrimitiveComponent* TouchedComponent)override;
	UFUNCTION()
	virtual void EndCursorOver(UPrimitiveComponent* TouchedComponent)override;

private:
	UPROPERTY()
	class UStaticMesh* EarthMesh;

	UPROPERTY()
	class UMaterialInterface* EarthMaterial;

	int32 bCanRotate : 1;
};
