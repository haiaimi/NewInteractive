// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Common/TestProjectHelper.h"
#include "Gameplay/MainController.h"
#include "TestProject.h"
#include "Locker.h"
#include "Engine/Engine.h"
#include "SPopMenuWidget.h"

// Sets default values
AInventoryActor::AInventoryActor():
	PopMenu(nullptr),
	bDestroyedFromPopMenu(false)
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
		//ActorMesh->OnBeginCursorOver.AddDynamic(this, &AInventoryActor::BeginCursorOver);
		//ActorMesh->OnEndCursorOver.AddDynamic(this, &AInventoryActor::EndCursorOver);
	}

	OwnerController = nullptr;
	MovePlane = FPlane(ForceInit);
	MoveWithCursor = false;
	bInLocker = false;

	FOnClicked ClickDelegate;
	ClickDelegate.BindUObject(this, &AInventoryActor::DestroySelf);

	InfoInMenu.Delegates[0] = ClickDelegate;
	InfoInMenu.ButtonNames[0] = TEXT("删除物体");
}

// Called when the game starts or when spawned
void AInventoryActor::BeginPlay()
{
	Super::BeginPlay();
	
	OriginLocation = GetRelativeLocation();
}

// Called every frame
void AInventoryActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MoveWithCursor)
	{
		FVector2D MousePosition;
		OwnerController->GetMousePosition(MousePosition.X, MousePosition.Y);
		FVector WorldPosition, WorldDirection;
		TestProjectHelper::DeprojectScreenToWorld_Cursor(OwnerController, WorldPosition, WorldDirection);      //获取鼠标所对应的射线
		
		FVector IntersectionPos = FMath::LinePlaneIntersection(WorldPosition, WorldPosition + 1000.f*WorldDirection, MovePlane);    //计算射线与平面的交点
		SetActorLocation(IntersectionPos + Offset);
	}

	MoveTick(DeltaTime);

	// 测试四元数旋转
	/*FVector CamDir = FRotator(-70.f, 0.f, 0.f).Vector();
	FQuat Rot(CamDir, 10.f * DeltaTime);

	AddActorWorldRotation(Rot);*/
}

void AInventoryActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AInventoryActor::GetTouchTypes_Implementation(TArray<TEnumAsByte<ECustomTouchType::Type>>& OutTypes)
{
	OutTypes = SupportTouchType;
}

void AInventoryActor::AddTouchTypes_Implementation(ECustomTouchType::Type InType)
{
	SupportTouchType.AddUnique(InType);

	TestProjectHelper::Debug_ScreenMessage(FString::FormatAsNumber(SupportTouchType.Find(InType)));
}

void AInventoryActor::RemoveTouchTypes_Implementation(ECustomTouchType::Type InType)
{
	if (InType == ECustomTouchType::AllTouchType)
	{
		SupportTouchType.Empty(0);
	}
	else
		SupportTouchType.Remove(InType);
}

void AInventoryActor::CanSuitTargetTouchType_Implementation(ECustomTouchType::Type TargetTouchType, bool& Out)
{
	int Temp;
	Out = SupportTouchType.Find(TargetTouchType, Temp);
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

FVector AInventoryActor::GetRelativeLocation()
{
	FVector RelativeLocation(ForceInit);

	if (GetAttachParentActor())
	{
		RelativeLocation = GetActorLocation() - GetAttachParentActor()->GetActorLocation();
		//TestProjectHelper::Debug_ScreenMessage(FString::Printf(TEXT("Inventory Location: %s, LockerLocation: %s, DestLocation: %s"), *GetActorLocation().ToString(), *GetAttachParentActor()->GetActorLocation().ToString(), *DestLocation.ToString()));
	}

	RelativeLocation.Y /= 1.5f;
	return RelativeLocation;
}

void AInventoryActor::ShowHighlight(bool bShow)
{
	ActorMesh->SetRenderCustomDepth(bShow);
}

FReply AInventoryActor::DestroySelf()
{
	bDestroyedFromPopMenu = true;
	AMainController* MyController = Cast<AMainController>(GetWorld()->GetFirstPlayerController());
	TArray<AActor*>& MultiSelectedActor = MyController->GetMultiSelectedActors();
	if (MultiSelectedActor.Num() > 0)
	{
		for (TArray<AActor*>::TIterator Iter(MultiSelectedActor); Iter; ++Iter)
		{
			(*Iter)->Destroy();
		}
	}
	else
		Super::Destroy();

	return FReply::Handled();
}

void AInventoryActor::Destroyed()
{
	Super::Destroyed();

	if (!bDestroyedFromPopMenu)
		return;
	AMainController* MyController = Cast<AMainController>(GetWorld()->GetFirstPlayerController());
	TSharedPtr<SPopMenuWidget>& PopMenu1 = MyController->GetPopMenu();
	if (PopMenu1.IsValid())
		PopMenu1.Reset();
}

void AInventoryActor::BeginMove(const FVector& DestLoc)
{
	bIsInMove = true;
	DestLocation = DestLoc;
}

void AInventoryActor::MoveTick(float DeltaTime)
{
	if (bIsInMove)  //此时物体需要移动
	{
		FVector NewLocation = FMath::VInterpTo(GetRelativeLocation(), DestLocation, DeltaTime, 10.f);
		SetActorRelativeLocation(NewLocation);
		CurRelativeLoc = NewLocation;
		
		if ((NewLocation - DestLocation).Size() < 0.5f)
		{
			SetActorRelativeLocation(DestLocation);
			bIsInMove = false;
			OriginLocation = DestLocation;
		}
	}
}
