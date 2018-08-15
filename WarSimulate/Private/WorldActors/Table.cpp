// Fill out your copyright notice in the Description page of Project Settings.

#include "Table.h"
#include "Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"


// Sets default values
ATable::ATable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TableMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TableMesh"));
	BaseSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("BaseScene"));    //创建基本根组件
	//查找资源
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/StarterContent/Architecture/Floor_400x400"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Game/StarterContent/Materials/M_Wood_Pine"));

	if (!ensure(MeshFinder.Succeeded()))return;
	TableMesh = MeshFinder.Object;

	if (!ensure(MaterialFinder.Succeeded()))return;
	TableMaterial = MaterialFinder.Object;

	//if (BaseSceneComponent)
	//	RootComponent = BaseSceneComponent;

	if (TableMeshComponent && BaseSceneComponent)
	{
		RootComponent = BaseSceneComponent;
		TableMeshComponent->SetStaticMesh(TableMesh);
		TableMeshComponent->SetMaterial(0, TableMaterial);
		TableMeshComponent->SetupAttachment(RootComponent);
		// 把局部坐标中心移到中心
		FVector TableExtent = TableMeshComponent->Bounds.BoxExtent;
		TableMeshComponent->SetRelativeLocation(FVector(-TableExtent.X, -TableExtent.Y, 0.f));
		//鼠标指针事件监听
		TableMeshComponent->OnBeginCursorOver.AddDynamic(this, &ATable::BeginCursorOver);
		TableMeshComponent->OnEndCursorOver.AddDynamic(this, &ATable::EndCursorOver);
	}
}

// Called when the game starts or when spawned
void ATable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATable::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ATable::ToggleMaterial(UMaterialInterface* InMaterial)
{
	TableMeshComponent->SetMaterial(0, InMaterial);
}

void ATable::BeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
	SetActorScale3D(FVector(1.01f, 1.01f, 1.01f));
}

void ATable::EndCursorOver(UPrimitiveComponent* TouchedComponent)
{
	SetActorScale3D(FVector(1.f, 1.f, 1.f));
}

