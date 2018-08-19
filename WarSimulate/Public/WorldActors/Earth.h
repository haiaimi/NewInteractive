// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldActors/InventoryActor.h"
#include "Earth.generated.h"

/**
 * 
 */
UCLASS()
class WARSIMULATE_API AEarth : public AInventoryActor
{
	GENERATED_BODY()

public:
	AEarth(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay()override;

public:
	virtual void Tick(float DeltaTime)override;

	virtual void PostInitializeComponents()override;

public:
	UFUNCTION()
	virtual void BeginCursorOver(UPrimitiveComponent* TouchedComponent)override;
	UFUNCTION()
	virtual void EndCursorOver(UPrimitiveComponent* TouchedComponent)override;

private:
	int32 bCanRotate : 1;
};
