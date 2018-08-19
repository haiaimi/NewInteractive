// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CommunicationManager.generated.h"

class AInventoryActor;
/**
 * 
 */
UCLASS()
class WARSIMULATE_API UCommunicationManager : public UObject
{
	GENERATED_BODY()
	
public:
	UCommunicationManager();

public:
	/** 构建SQBActor之间的通信*/
	void BuildCommunication(AInventoryActor* Sender, AInventoryActor* Receiver, FString& FunName);

	/** 结束通信*/
	void BreakCommunication(AInventoryActor* Sender, AInventoryActor* Receiver, FString FunName);
	
	
};
