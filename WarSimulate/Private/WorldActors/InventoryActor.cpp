// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldActors/InventoryActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Common/OriginHelper.h"
#include "Gameplay/PlatformController.h"
#include "WarSimulateProject.h"
#include "Locker.h"
#include "Engine/Engine.h"
#include "SPopMenuWidget.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/FloatingPawnMovement.h"

TArray<UFunction*> GlobalModuleFunctions = { nullptr };
TMap<FName, BindFunctionPtr> GlobalBindFunctions = {};
TMap<FName, RemoveDelegatePtr> GlobalRemoveDelegates = {};
typedef void(*funPtr)(int32, int32);

// Sets default values
AInventoryActor::AInventoryActor(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer),
	PopMenu(nullptr),
	bDestroyedFromPopMenu(false),
	CommunicateType(EOutsideCommunicate::ELoadConfigFile_Json),    //默认是读取Json文件的方式
	OwnerPltform(nullptr),
	PlatformType(EPlatformCategory::EBaseModule)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ModuleMovement = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("ModuleMovement"));
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	BaseScene = CreateDefaultSubobject<USceneComponent>(TEXT("BaseScene"));
	ActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ActorMesh"));
	ViewCamera->SetupAttachment(BaseScene);
	PlatformData = new FInventoryActorData;

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

	InfoInMenu.AddButton(FString(TEXT("删除物体")), ClickDelegate);
}

// Called when the game starts or when spawned
void AInventoryActor::BeginPlay()
{
	Super::BeginPlay();
	
	WH_FUN_DEFINE_IMPLEMENT();
	OriginLocation = GetRelativeLocation();

	//更新通信状态，执行对应通信状态的方法
	UpdateCommunicateType();   

	for (auto Iter = GlobalBindFunctions.CreateIterator(); Iter; ++Iter)
	{
		OriginHelper::Debug_ScreenMessage((*Iter).Key.ToString());
	}

	OriginHelper::Debug_ScreenMessage(FString::Printf(TEXT("%p"), &SendPosInfoBindPtr), 10);
	OriginHelper::Debug_ScreenMessage(FString::Printf(TEXT("%p"), &SendIDBindPtr), 10);
	OriginHelper::Debug_ScreenMessage(FString::FormatAsNumber(sizeof(FVector)));
	OriginHelper::Debug_ScreenMessage(FString::FormatAsNumber(sizeof(ICustomTouchInterface)));
	OriginHelper::Debug_ScreenMessage(FString::FormatAsNumber(sizeof(FSendPosInfoDelegate)));
	OriginHelper::Debug_ScreenMessage(FString::FormatAsNumber((int32)(&DefineEnd - &DefineStart)));
}

void AInventoryActor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//设置基础模块的玩家输入响应
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("BaseMoveForward", EKeys::W, 1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("BaseMoveForward", EKeys::S, -1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("BaseMoveRight", EKeys::D, 1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("BaseMoveRight", EKeys::A, -1.f));

	PlayerInputComponent->BindAxis(TEXT("BaseMoveForward"), this, &AInventoryActor::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("BaseMoveRight"), this, &AInventoryActor::MoveRight);
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
		OriginHelper::DeprojectScreenToWorld_Cursor(OwnerController, WorldPosition, WorldDirection);      //获取鼠标所对应的射线
		
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

void AInventoryActor::BeginDestroy()
{
	Super::BeginDestroy();

	delete PlatformData;
	PlatformData = nullptr;
}

void AInventoryActor::GetTouchTypes_Implementation(TArray<TEnumAsByte<ECustomTouchType::Type>>& OutTypes)
{
	OutTypes = SupportTouchType;
}

void AInventoryActor::AddTouchTypes_Implementation(ECustomTouchType::Type InType)
{
	SupportTouchType.AddUnique(InType);

	OriginHelper::Debug_ScreenMessage(FString::FormatAsNumber(SupportTouchType.Find(InType)));
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

void AInventoryActor::StartMoveWithCursor(class APlatformController* Owner, const FVector Offset, const FPlane MovePalne)
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
	APlatformController* MyController = Cast<APlatformController>(GetWorld()->GetFirstPlayerController());
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
	APlatformController* MyController = Cast<APlatformController>(GetWorld()->GetFirstPlayerController());
	TSharedPtr<SPopMenuWidget>& PopMenu1 = MyController->GetPopMenu();
	if (PopMenu1.IsValid())
		PopMenu1.Reset();
}

void AInventoryActor::BeginMove(const FVector& DestLoc)
{
	bIsInMove = true;
	DestLocation = DestLoc;
}

void AInventoryActor::SetCommunicateType(EOutsideCommunicate::Type InType)
{
	CommunicateType = InType;
	//更新通信模式，不同的模块会重写不同的方法
	UpdateCommunicateType();  
}

void AInventoryActor::SetMaxSpeed(float InSpeed)
{
	UFloatingPawnMovement* CurMovement = Cast<UFloatingPawnMovement>(ModuleMovement);
	CurMovement->MaxSpeed = InSpeed;
}

void AInventoryActor::MoveForward(float Val)
{
	MoveForwardImpl(Val);
}

void AInventoryActor::MoveRight(float Val)
{
	MoveRightImpl(Val);
}

void AInventoryActor::MoveForwardImpl(float Val)
{
	AddMovementInput(GetControlRotation().Vector(), Val);
}

void AInventoryActor::MoveRightImpl(float Val)
{
	FRotator ControlRot = GetControlRotation();
	AddMovementInput(FRotationMatrix(ControlRot).GetUnitAxis(EAxis::Y), Val);
}

void AInventoryActor::SetPlatformData(FName InID, ESQBTeam::Type InTeam)
{
	PlatformData->ID = InID;
	PlatformData->OwnerTeam = InTeam;
}

void AInventoryActor::UpdatePlatformData()
{

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
