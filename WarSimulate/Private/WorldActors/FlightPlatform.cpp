// Fill out your copyright notice in the Description page of Project Settings.

#include "FlightPlatform.h"
#include "Components/StaticMeshComponent.h"



AFlightPlatform::AFlightPlatform(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Plane/CGModel/F18C/F-18C_Hornet"));
	if (!ensure(MeshFinder.Succeeded()))return;
	ActorMesh->SetStaticMesh(MeshFinder.Object);

	SupportTouchType.Add(ECustomTouchType::Drag_1P);
}
