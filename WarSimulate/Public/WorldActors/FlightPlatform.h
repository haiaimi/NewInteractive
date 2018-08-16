// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldActors/InventoryActor.h"
#include "FlightPlatform.generated.h"

/**
 *  飞行平台
 */
UCLASS()
class WARSIMULATE_API AFlightPlatform : public AInventoryActor
{
	GENERATED_BODY()

	AFlightPlatform(const FObjectInitializer& ObjectInitializer);
	
public:
	/**飞机网格模型*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent * PlaneMesh;
};
