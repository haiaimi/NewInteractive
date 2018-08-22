// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WarSimulateType.h"
#include "Communication/CommunicationManager.h"
#include "InventoryActor.h" 
#include "WarSimulateInstance.generated.h"

class AInventoryActor;
class UCommunicationManager;
/**
 * 游戏Instance类，用于BaseActor之间的通信,是通信的中间类
 */
UCLASS()
class WARSIMULATE_API UWarSimulateInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UWarSimulateInstance();

	virtual void StartGameInstance()override;

public:
	/** 注册BaseActor，以便相互之间的通信*/
	void RegisterSQBActor(AInventoryActor* InRef);

	/** 注销列表中存在的Actor，此时该Actor已被摧毁或者不参与通信交互*/
	void UnRegisterSQBActor(AInventoryActor* InRef);

	template<class T>
	bool SpawnSQBActor(UClass* InClass, FVector const* Location = NULL, FRotator const* Rotation = NULL, const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters())
	{
		T* SpawnedActor = GetWorld()->SpawnActor<T>(Location, Rotation, SpawnParameters);
		if (SpawnedActor)
		{
			if (SpawnedActor->GetClass()->IsChildOf(AInventoryActor::StaticClass()))
			{
				RegisterSQBActor(SpawnedActor);
				return true;
			}
		}
		return false;      //生成失败
	}

	/** 获取所有BaseActor的ID名*/
	TArray<FName> GetData_AllOtherName(FName& PlatformID, AInventoryActor* ActorRef);

	WH_COMMUNICATE_IMPLEMENT(SendPosInfo);

private:
	TArray<AInventoryActor*> AllSQBActor;

	/**所有SQBActor相关的数据*/
	TArray<struct FInventoryActorData*> AllSQBData;

	/**内部通信管理类*/
	UCommunicationManager* CommunicationManager;
};
