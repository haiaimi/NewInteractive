// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldActors/InventoryActor.h"
#include "BattleField.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJECT_API ABattleField : public AInventoryActor
{
	GENERATED_BODY()
	
	
public:
	ABattleField();

	virtual void BeginPlay()override;

	virtual void Tick(float DeltaTime)override;

private:
	class ULandscapeComponent* Landscape;
	
};
