// Fill out your copyright notice in the Description page of Project Settings.

#include "MainController.h"
#include "ConstructorHelpers.h"
#include "Table.h"
#include "Engine/Engine.h"


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
	MaterialIndex = 0;    //默认材质目录
	bEnableMouseOverEvents = true;    //启用鼠标覆盖事件检测
}

void AMainController::BeginPlay()
{
	Super::BeginPlay();

	//在视口前方设置一个桌面
	//MaterialIndex		
	FVector ViewLocation;
	FRotator ViewRotation;
	PlayerCameraManager->GetViewTarget()->GetActorEyesViewPoint(ViewLocation, ViewRotation);  //获取摄像头的位置及方向
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::Printf(TEXT("Camera Position:%s, Camera Rotation:%s"), *ViewLocation.ToString(), *ViewRotation.Vector().ToString()));   //测试位置

	//桌面生成的具体位置
	const FMatrix TableRotMat = FRotationMatrix(ViewRotation);
	const FVector TableX2D = TableRotMat.GetUnitAxis(EAxis::X).GetSafeNormal2D();
	const FVector TableY = TableRotMat.GetUnitAxis(EAxis::Y).GetSafeNormal();

	const FVector TableSapwnLoaction = ViewLocation + ViewRotation.Vector()*500.f;
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;
	
	ATable* SpawnedTable = GetWorld()->SpawnActor<ATable>(TableSapwnLoaction, FRotator::ZeroRotator, SpawnParam);
	if (SpawnedTable)
		CurTable = SpawnedTable;
}

void AMainController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	float x, y;
	GetMousePosition(x, y);
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Black, FVector2D(x, y).ToString());
}

void AMainController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindAction(TEXT("ToggleMaterial"), EInputEvent::IE_Pressed, this, &AMainController::ToggleTableMaterial);
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