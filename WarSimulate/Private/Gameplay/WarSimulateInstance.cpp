// Fill out your copyright notice in the Description page of Project Settings.

#include "WarSimulateInstance.h"
#include "OriginHelper.h"
#include "FlightPlatform.h"


UWarSimulateInstance::UWarSimulateInstance():
	CommunicationManager(nullptr)
{

}

void UWarSimulateInstance::StartGameInstance()
{
	

	Super::StartGameInstance();
}

void UWarSimulateInstance::OnStart()
{
	Super::OnStart();

	if (CommunicationManager == nullptr)
		CommunicationManager = NewObject<UCommunicationManager>(this);
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

void UWarSimulateInstance::SendPosInfo_Implementation(AInventoryActor* Sender, FSendPosInfo& InParams)
{
	InParams.PlatformPos = Sender->GetActorLocation() + Sender->GetActorRotation().Vector()*100.f;
	InParams.FlySpeed = Cast<AFlightPlatform>(Sender)->GetFlySpeed();
	
	Sender->SendPosInfo_Publish(&InParams);
}

