git // Fill out your copyright notice in the Description page of Project Settings.

#include "WarSimulateInstance.h"
#include "OriginHelper.h"
#include "InventoryActor.h" 
#include "FlightPlatform.h"
#include "Communication/CommunicationManager.h"


UWarSimulateInstance::UWarSimulateInstance():
	CommunicationManager(nullptr)
{

}

void UWarSimulateInstance::StartGameInstance()
{
	if (CommunicationManager == nullptr)
		CommunicationManager = NewObject<UCommunicationManager>(this);

	CommunicationManager->BreakCommunication(nullptr, nullptr, FString(TEXT("SendID")));

	Super::StartGameInstance();
}

void UWarSimulateInstance::RegisterSQBActor(class AInventoryActor* InRef)
{
	AllSQBActor.Add(InRef);
}

void UWarSimulateInstance::UnRegisterSQBActor(class AInventoryActor* InRef)
{

}

TArray<FName> UWarSimulateInstance::GetData_AllOtherName(FName& PlatformID, class AInventoryActor* ActorRef)
{
	TArray<FName> Result;
	for (auto Iter : AllSQBActor)
	{
		Result.Add(Iter->GetPlatformData()->ID);
	}

	return Result;
}

//void UWarSimulateInstance::SendPosInfo_Implementation(AInventoryActor* Sender, void* InParams)
//{
//	FSendPosInfo Temp;
//	if (InParams)
//	{
//		Temp.PlatformPos = Sender->GetActorLocation() + Sender->GetActorRotation().Vector()*100.f;
//		Temp.FlySpeed = Cast<AFlightPlatform>(Sender)->GetFlySpeed();
//	}
//
//	Sender->SendPosInfo_Publish(&Temp);
//}

