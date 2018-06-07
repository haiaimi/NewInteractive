﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "MainController.h"
#include "ConstructorHelpers.h"
#include "Table.h"
#include "Engine/Engine.h"
#include "Common/TestProjectHelper.h"
#include "Engine/LocalPlayer.h"
#include "SceneView.h"
#include "InventoryActor.h"


AMainController::AMainController()
{
	PrimaryActorTick.bCanEverTick = true;      //Controller每帧更新
	MaterialInventory.Reset();

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material_1(TEXT("/Game/StarterContent/Materials/M_Wood_Pine"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material_2(TEXT("/Game/StarterContent/Materials/M_Wood_Oak"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material_3(TEXT("/Game/StarterContent/Materials/M_Wood_Walnut"));

	if (Material_1.Succeeded())MaterialInventory.Add(Material_1.Object);
	if (Material_2.Succeeded())MaterialInventory.Add(Material_2.Object);
	if (Material_3.Succeeded())MaterialInventory.Add(Material_3.Object);

	CurTable = nullptr;
	CurDragThing = nullptr;
	MaterialIndex = 0;    //默认材质目录
	bEnableMouseOverEvents = true;    //启用鼠标覆盖事件检测
}

void AMainController::BeginPlay()
{
	Super::BeginPlay();

	//把鼠标指针限制在Viewport里
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	GetLocalPlayer()->ViewportClient->SetCaptureMouseOnClick(EMouseCaptureMode::CaptureDuringMouseDown);    //鼠标按下时就监听事件，不然需要双击
	GetLocalPlayer()->ViewportClient->SetMouseLockMode(EMouseLockMode::LockAlways);

	//在视口前方设置一个桌面	
	FVector ViewLocation;
	FRotator ViewRotation;
	PlayerCameraManager->GetViewTarget()->GetActorEyesViewPoint(ViewLocation, ViewRotation);  //获取摄像头的位置及方向
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::Printf(TEXT("Camera Position:%s, Camera Rotation:%s"), *ViewLocation.ToString(), *ViewRotation.Vector().ToString()));   //测试位置

	//桌面生成的具体位置
	const FMatrix TableRotMat = FRotationMatrix(ViewRotation);
	const FVector TableX2D = TableRotMat.GetUnitAxis(EAxis::X).GetSafeNormal2D();
	const FVector TableY = TableRotMat.GetUnitAxis(EAxis::Y).GetSafeNormal();

	TestProjectHelper::Debug_ScreenMessage(TableX2D.ToString());
	const FVector TableSapwnLoaction = ViewLocation - TableX2D * 200.f + ViewRotation.Vector()*500.f;
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;
	
	ATable* SpawnedTable = GetWorld()->SpawnActor<ATable>(TableSapwnLoaction, FRotator::ZeroRotator, SpawnParam);
	if (SpawnedTable)
	{
		CurTable = SpawnedTable;
	}
}

void AMainController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	float x, y;
	GetMousePosition(x, y);
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Black, FVector2D(x, y).ToString());
}

void AMainController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindAction(TEXT("ToggleMaterial"), EInputEvent::IE_Pressed, this, &AMainController::ToggleTableMaterial);
		InputComponent->BindAction(TEXT("Drag"), EInputEvent::IE_Pressed, this, &AMainController::DragSomeThing);
		InputComponent->BindAction(TEXT("Drag"), EInputEvent::IE_Released, this, &AMainController::StopDrag);
	}
}

void AMainController::ToggleTableMaterial()
{
	if (CurTable)
	{
		MaterialIndex = (MaterialIndex + 1) % 3;
		CurTable->ToggleMaterial(MaterialInventory[MaterialIndex]);
	}
}

void AMainController::DragSomeThing()
{
	FVector WorldPosition, WorldDirection;
	TestProjectHelper::DeProjectScreenToWorld(this, WorldPosition, WorldDirection);

	FHitResult HitResult;
	//FCollisionQueryParams
	GetWorld()->LineTraceSingleByChannel(HitResult, WorldPosition, WorldPosition + WorldDirection * 1000.f, ECollisionChannel::ECC_WorldStatic);

	if (HitResult.GetActor())
	{
		AInventoryActor* HitActor = nullptr;
		HitActor = Cast<AInventoryActor>(HitResult.GetActor());
		if (HitActor && CurTable)
		{
			TestProjectHelper::Debug_ScreenMessage(TEXT("Get it!!"));
			CurDragThing = HitActor;
			const FVector Offset = CurDragThing->GetActorLocation() - HitResult.ImpactPoint;   //鼠标指针相对于物体的位置
			const FPlane MovePlane(HitResult.ImpactPoint, FRotationMatrix(CurDragThing->GetActorRotation()).GetUnitAxis(EAxis::Z));   //获取鼠标与物体撞击点的平面
			CurDragThing->StartMoveWithCursor(this, Offset, MovePlane);
		}
	}
}

void AMainController::StopDrag()
{
	if (CurDragThing)
	{
		CurDragThing->StopMoveWithCursor();
		CurDragThing = nullptr;
	}
}