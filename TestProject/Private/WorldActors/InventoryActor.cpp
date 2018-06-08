// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Common/TestProjectHelper.h"
#include "TestProject.h"
#include "Locker.h"


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
		ActorMesh->SetCollisionResponseToChannel(COLLISION_LOCKERRAY, ECollisionResponse::ECR_Ignore);
		ActorMesh->SetCollisionResponseToChannel(COLLISION_INVENTORYRAY, ECollisionResponse::ECR_Block);
		ActorMesh->SetupAttachment(RootComponent);
		ActorMesh->OnBeginCursorOver.AddDynamic(this, &AInventoryActor::BeginCursorOver);
		ActorMesh->OnEndCursorOver.AddDynamic(this, &AInventoryActor::EndCursorOver);
	}

	OwnerController = nullptr;
	MovePlane = FPlane(ForceInit);
	MoveWithCursor = false;
	bInLocker = false;
}

// Called when the game starts or when spawned
void AInventoryActor::BeginPlay()
{
	Super::BeginPlay();
	
	OriginLocation = GetActorLocation();
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

	MoveTick(DeltaTime);
}

void AInventoryActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AInventoryActor::BeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
	// 对物体进行光照
	if (bInLocker)
	{
		if (ALocker* OwnerLocker = Cast<ALocker>(GetAttachParentActor()))
		{
			OwnerLocker->CastLight(this);
		}
	}
}

void AInventoryActor::EndCursorOver(UPrimitiveComponent* TouchedComponent)
{
	if (bInLocker)
	{
		if (ALocker* OwnerLocker = Cast<ALocker>(GetAttachParentActor()))
		{
			OwnerLocker->StopCastLight();
		}
	}
	
}

float AInventoryActor::GetHeight()
{
	return Height;
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

void AInventoryActor::MoveTick(float DeltaTime)
{
	if (bIsInMove)  //此时物体需要移动
	{
		FVector NewLocation = FMath::VInterpTo(GetActorLocation(), DestLocation, DeltaTime, 10.f);
		SetActorLocation(NewLocation);

		if ((NewLocation - DestLocation).Size() < 0.5f)
		{
			SetActorLocation(DestLocation);
			bIsInMove = false;
			OriginLocation = DestLocation;
		}
	}
}
