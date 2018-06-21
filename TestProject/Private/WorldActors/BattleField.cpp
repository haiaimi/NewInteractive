// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleField.h"
#include "LandscapeComponent.h"

ABattleField::ABattleField()
{
	//Landscape = CreateDefaultSubobject<ULandscapeComponent>(TEXT("BattleField"));
}

void ABattleField::BeginPlay()
{
	Super::BeginPlay();
}

void ABattleField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}