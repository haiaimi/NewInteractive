﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawnMovement.h"
#include "GroundSpectatorPawnMovement.generated.h"

/**
 * 
 */
UCLASS()
class WARSIMULATE_API UGroundSpectatorPawnMovement : public USpectatorPawnMovement
{
	GENERATED_BODY()
	
	
public:
	void Move() {};
	
};
