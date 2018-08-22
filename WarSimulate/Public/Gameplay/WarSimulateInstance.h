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
 * ��ϷInstance�࣬����BaseActor֮���ͨ��,��ͨ�ŵ��м���
 */
UCLASS()
class WARSIMULATE_API UWarSimulateInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UWarSimulateInstance();

	virtual void StartGameInstance()override;

public:
	/** ע��BaseActor���Ա��໥֮���ͨ��*/
	void RegisterSQBActor(AInventoryActor* InRef);

	/** ע���б��д��ڵ�Actor����ʱ��Actor�ѱ��ݻٻ��߲�����ͨ�Ž���*/
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
		return false;      //����ʧ��
	}

	/** ��ȡ����BaseActor��ID��*/
	TArray<FName> GetData_AllOtherName(FName& PlatformID, AInventoryActor* ActorRef);

	WH_COMMUNICATE_IMPLEMENT(SendPosInfo);

private:
	TArray<AInventoryActor*> AllSQBActor;

	/**����SQBActor��ص�����*/
	TArray<struct FInventoryActorData*> AllSQBData;

	/**�ڲ�ͨ�Ź�����*/
	UCommunicationManager* CommunicationManager;
};
