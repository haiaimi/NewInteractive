// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "GroundSpectatorPawn.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJECT_API AGroundSpectatorPawn : public ASpectatorPawn
{
	GENERATED_UCLASS_BODY()
	
public:
	//继承于父类的方法
	virtual void MoveRight(float Val)override;

	virtual void MoveForward(float Val)override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent)override;

	class UGroundCameraComponent* GetGroundCameraComponent()const;

private:
	/**用于观察的相机*/
	class UGroundCameraComponent* SpectatorCamera;

};
