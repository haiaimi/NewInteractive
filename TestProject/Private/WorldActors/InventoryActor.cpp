// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Common/TestProjectHelper.h"


// Sets default values
AInventoryActor::AInventoryActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ActorMesh"));
	BaseScene = CreateDefaultSubobject<USceneComponent>(TEXT("BaseScene"));

	if (BaseScene)
		RootComponent = BaseScene;
	if (ActorMesh)
	{
		ActorMesh->SetupAttachment(RootComponent);
		ActorMesh->OnBeginCursorOver.AddDynamic(this, &AInventoryActor::BeginCursorOver);
		ActorMesh->OnEndCursorOver.AddDynamic(this, &AInventoryActor::EndCursorOver);
	}

	OwnerController = nullptr;
	MovePlane = FPlane(ForceInit);
	MoveWithCursor = false;
}

// Called when the game starts or when spawned
void AInventoryActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInventoryActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MoveWithCursor)
	{
		FVector WorldPosition, WorldDirection;
		TestProjectHelper::DeProjectScreenToWorld(OwnerController, WorldPosition, WorldDirection);      //获取鼠标所对应的射线

		FVector IntersectionPos = FMath::LinePlaneIntersection(WorldPosition, WorldPosition + 1000.f*WorldDirection, MovePlane);    //计算射线与平面的交点
		SetActorLocation(IntersectionPos + Offset);
	}
}

void AInventoryActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AInventoryActor::BeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
	
}

void AInventoryActor::EndCursorOver(UPrimitiveComponent* TouchedComponent)
{
	
}

void AInventoryActor::StartMoveWithCursor(class AMainController* Owner, const FVector Offset, const FPlane MovePalne)
{
	this->OwnerController = Owner;
	this->MovePlane = MovePalne;
	this->Offset = Offset;    //设置偏移量

	MoveWithCursor = true;     //随着鼠标指针移动
}

void AInventoryActor::StopMoveWithCursor()
{
	MoveWithCursor = false;
	Offset = FVector::ZeroVector;
	MovePlane = FPlane(ForceInit);
	OwnerController = nullptr;
}

