// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldActors/InventoryActor.h"
#include "FlightPlatform.generated.h"

/**
 *  ����ƽ̨
 */
UCLASS()
class WARSIMULATE_API AFlightPlatform : public AInventoryActor
{
	GENERATED_BODY()

	AFlightPlatform(const FObjectInitializer& ObjectInitializer);
	
public:
	/**�ɻ�����ģ��*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent * PlaneMesh;
};
