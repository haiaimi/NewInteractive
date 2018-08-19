// Fill out your copyright notice in the Description page of Project Settings.

#include "Communication/CommunicationManager.h"
#include "InventoryActor.h"

UCommunicationManager::UCommunicationManager()
{

}

void UCommunicationManager::BuildCommunication(class AInventoryActor* PublishActor, class AInventoryActor* SubscribeActor, FString& FunName)
{
	if (BindFunctionPtr* FindRes = GlobalBindFunctions.Find(FName(*FunName)))
	{
		// 调用函数指针进行绑定
		FDelegateHandle BindHandle = (PublishActor->**FindRes)(SubscribeActor);   //执行该函数指针，进行绑定，并获取其代理句柄
	}
}

void UCommunicationManager::BreakCommunication(AInventoryActor* Sender, AInventoryActor* Receiver, FString FunName)
{
	FName TempName(*FunName);
	RemoveDelegatePtr* RemoveDelegate = GlobalRemoveDelegates.Find(TempName);

	if (RemoveDelegate)
	{
		// 下面就是调用函数指针
		(Sender->**RemoveDelegate)(Receiver);
	}
}

