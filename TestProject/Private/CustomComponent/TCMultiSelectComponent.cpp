// Fill out your copyright notice in the Description page of Project Settings.

#include "TCMultiSelectComponent.h"


// Sets default values for this component's properties
UTCMultiSelectComponent::UTCMultiSelectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTCMultiSelectComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTCMultiSelectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

