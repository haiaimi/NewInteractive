// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/MainController.h"
#include "ConstructorHelpers.h"
#include "Table.h"
#include "Engine/Engine.h"
#include "Common/TestProjectHelper.h"
#include "Engine/LocalPlayer.h"
#include "SceneView.h"
#include "InventoryActor.h"
#include "Locker.h"
#include "TestProject.h"
#include "Engine/World.h"
#include "UI/TestProjectHUD.h"
#include "Kismet/GameplayStatics.h"
#include "../../Public/Gameplay/GroundCameraComponent.h"
#include "SlateApplication.h"
#include "TimerManager.h"
#include "Gameplay/CustomTouchInput.h"


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
	CurLocker = nullptr;
	CurDragThing = nullptr;
	CurMenuSpawnThing = nullptr;
	InputHandle = nullptr;
	MaterialIndex = 0;    //默认材质目录
	bEnableMouseOverEvents = true;    //启用鼠标覆盖事件检测
	bEnableTouchEvents = true;   //启用触摸事件
	bEnableTouchOverEvents = true;
	bShowMouseCursor = true;
}

void AMainController::BeginPlay()
{
	Super::BeginPlay();

	InputHandle = NewObject<UCustomTouchInput>(this);
	//把鼠标指针限制在Viewport里
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	GetLocalPlayer()->ViewportClient->SetCaptureMouseOnClick(EMouseCaptureMode::CaptureDuringMouseDown);    //鼠标按下时就监听事件，不然需要双击
	GetLocalPlayer()->ViewportClient->SetMouseLockMode(EMouseLockMode::LockAlways);

	//GetPawn()->SetActorHiddenInGame(true);
	
	//在视口前方设置一个桌面	
	FVector ViewLocation;
	FRotator ViewRotation;
	PlayerCameraManager->GetViewTarget()->GetActorEyesViewPoint(ViewLocation, ViewRotation);  //获取摄像头的位置及方向
	FRotator LookRotation = ((const UGroundCameraComponent*)UGroundCameraComponent::StaticClass()->GetDefaultObject())->LookRotation;
	//FRotator LookRotation = GetDefault<UGroundCameraComponent>(UGroundCameraComponent::StaticClass())->LookRotation;

	//桌面生成的具体位置
	const FMatrix TableRotMat = FRotationMatrix(LookRotation);
	const FVector TableX2D = TableRotMat.GetUnitAxis(EAxis::X).GetSafeNormal2D();
	const FVector TableY = TableRotMat.GetUnitAxis(EAxis::Y).GetSafeNormal();

	TestProjectHelper::Debug_ScreenMessage(LookRotation.ToString());
	const FVector TableSapwnLoaction = ViewLocation + TableX2D * -30.f + LookRotation.Vector() * 550.f;
	const FVector LockerSpawnLocation = ViewLocation + TableX2D * 170.f + LookRotation.Vector() * 530.f;
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;
	
	ATable* SpawnedTable = GetWorld()->SpawnActor<ATable>(TableSapwnLoaction, FRotator::ZeroRotator, SpawnParam);
	ALocker* SpawnedLocker = GetWorld()->SpawnActor<ALocker>(LockerSpawnLocation, FRotator::ZeroRotator, SpawnParam);

	if (SpawnedTable)
	{
		CurTable = SpawnedTable;
		if (GetPawn())
			CurTable->AttachToActor(GetPawn(), FAttachmentTransformRules::KeepWorldTransform);       //绑定到Pawn中
	}
	
	if (SpawnedLocker)
	{
		CurLocker = SpawnedLocker;
		CurLocker->AttachToActor(GetPawn(), FAttachmentTransformRules::KeepWorldTransform);       //绑定到Pawn中
		CurLocker->SetVisibility(false);    //关闭Locker可见性
		CurLocker->StopCastLight();
	}

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&]() 
	{
		if (CurLocker)
		{
			CurLocker->UpdateRelativePosToPawn(nullptr, 0);
			CurLocker->SetActorRelativeLocation(CurLocker->GetRelativeLocationToPawn_Hide());
			CurLocker->SetVisibility(true);
			CurLocker->Switch();

			if (InputHandle)
			{
				InputHandle->OnePointEvent[IE_Pressed].BindUObject(CurLocker, &ALocker::StartOpenLocker);
				InputHandle->OnePointEvent[IE_Released].BindUObject(CurLocker, &ALocker::EndOpenLocker);
				//InputHandle->OnePointEvent[IE_Repeat].BindUObject(CurLocker, &ALocker::UpdateMove);
				BIND_1P_ACTION(InputHandle, EGameTouchKey::Swipe, IE_Pressed, CurLocker, &ALocker::StartOpenLocker);
				BIND_1P_ACTION(InputHandle, EGameTouchKey::Swipe, IE_Released, CurLocker, &ALocker::EndOpenLocker);
			}
		}
	});
	GetWorldTimerManager().SetTimer(SpawnLockerHandle, TimerDelegate, 0.1f, false);
}

void AMainController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurTable)
	{
		//TestProjectHelper::Debug_ScreenMessage(CurTable->GetActorLocation().ToString());
	}
}

void AMainController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputHandle)
	{
	}

	if (InputComponent)
	{
		InputComponent->BindAction(TEXT("ToggleMaterial"), EInputEvent::IE_Pressed, this, &AMainController::ToggleTableMaterial);
		InputComponent->BindAction(TEXT("Drag"), EInputEvent::IE_Pressed, this, &AMainController::DragSomeThing);
		InputComponent->BindAction(TEXT("Drag"), EInputEvent::IE_Released, this, &AMainController::StopDrag);
		InputComponent->BindAction(TEXT("Quit"), EInputEvent::IE_Pressed, this, &AMainController::QuitGame);
		InputComponent->BindAction(TEXT("LockerSwitch"), EInputEvent::IE_Pressed, this, &AMainController::SwitchLocker);
	}
}

void AMainController::ProcessPlayerInput(const float DeltaTime, const bool bGamePaused)
{
	if (InputHandle && PlayerInput)
		InputHandle->UpdateInputStates(DeltaTime);

	Super::ProcessPlayerInput(DeltaTime, bGamePaused);
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
	TestProjectHelper::DeprojectScreenToWorld_Cursor(this, WorldPosition, WorldDirection);

	FHitResult HitResult;
	FCollisionQueryParams QueryParam;

	GetWorld()->LineTraceSingleByChannel(HitResult, WorldPosition, WorldPosition + WorldDirection * 1000.f, COLLISION_INVENTORYRAY, QueryParam);
	AInventoryActor* HitActor = nullptr;

	if (HitResult.GetActor())
	{
		HitActor = Cast<AInventoryActor>(HitResult.GetActor());
		if (HitActor && CurTable)
		{
			//TestProjectHelper::Debug_ScreenMessage(TEXT("Get it!!"));
			CurDragThing = HitActor;
			const FVector Offset = CurDragThing->GetActorLocation() - HitResult.ImpactPoint;   //鼠标指针相对于物体的位置
			const FPlane MovePlane(HitResult.ImpactPoint, FRotationMatrix(CurDragThing->GetActorRotation()).GetUnitAxis(EAxis::Z));   //获取鼠标与物体撞击点的平面
			CurDragThing->StartMoveWithCursor(this, Offset, MovePlane);
		}
	}

	if (ATestProjectHUD* CurHUD = Cast<ATestProjectHUD>(GetHUD()))
	{
		if (CurHUD->IsInventoryWidgetValid())
		{
			const bool bTriggerMenu = DoesCursorInMenu();
			if (bTriggerMenu)
			{
				CurHUD->ShowMenu(true);      //显示菜单
			}
			else if(!bTriggerMenu && !HitActor)
			{
				CurHUD->ShowMenu(false);       //可能会关闭菜单
			}
		}
	}
	
}

void AMainController::StopDrag()
{
	if (CurDragThing)
	{
		//检测是否停留在储物柜上，然后决定存放位置
		FVector WorldPosition, WorldDirection;
		TestProjectHelper::DeprojectScreenToWorld_Cursor(this, WorldPosition, WorldDirection);
		FHitResult HitResult;
		//FCollisionQueryParams QueryParam;
		//QueryParam.bIgnoreBlocks = true;  //忽略阻挡
		GetWorld()->LineTraceSingleByChannel(HitResult, WorldPosition, WorldPosition + WorldDirection * 1000.f, COLLISION_LOCKERRAY);
		
		if (ALocker* AimLocker = Cast<ALocker>(HitResult.GetActor()))
		{
			if (CurDragThing->bInLocker)
			{
				AimLocker->RemoveInventoryThing(CurDragThing);
				AimLocker->AddInventoryThing(CurDragThing, HitResult.ImpactPoint);
			}
			else
			{
				TestProjectHelper::Debug_ScreenMessage(TEXT("Catch Locker"));
				AimLocker->AddInventoryThing(CurDragThing, HitResult.ImpactPoint);
			}
		}
		else
		{
			if (ATestProjectHUD* CurHUD = Cast<ATestProjectHUD>(GetHUD()))
			{
				if (CurHUD->IsInventoryWidgetValid())
				{
					const bool bInMenu = DoesCursorInMenu();
					if (bInMenu && CurHUD->IsMenuShow() && CurMenuSpawnThing)      //菜单正在显示，并且鼠标指针在菜单内
					{
						CurMenuSpawnThing->Destroy();
					}
					else
					{
						CurDragThing->OriginLocation = CurDragThing->GetActorLocation();  //设置物体当前新的停留位置
						if (CurDragThing->bInLocker)
						{
							ALocker* OwnerLocker = Cast<ALocker>(CurDragThing->GetAttachParentActor());
							CurDragThing->bInLocker = false;

							if (OwnerLocker)
							{
								OwnerLocker->RemoveInventoryThing(CurDragThing);
								OwnerLocker->StopCastLight();
							}
							CurDragThing->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
						}
					}
				}
			}
		}
		FSlateApplication::Get().SetCursorPos(FVector2D(0.f, 0.f));       //在触屏状态下恢复鼠标位置
		CurDragThing->StopMoveWithCursor();
		CurDragThing = nullptr;
		CurMenuSpawnThing = nullptr;
	}
}

void AMainController::SpawnInventoryActors(UClass* SpawnedActor)
{
	if (GetWorld() && !CurMenuSpawnThing)      //如果当前已经控制着一个就不需要生成
	{
		FVector WorldPos, WorldDir;
		TestProjectHelper::DeprojectScreenToWorld_Cursor(this, WorldPos, WorldDir);
		FTransform ActorSpawnTransform = FTransform(FRotator::ZeroRotator, WorldPos + 600.f*WorldDir);
		CurMenuSpawnThing = Cast<AInventoryActor>(GetWorld()->SpawnActor(SpawnedActor, &ActorSpawnTransform));
	}
}

void AMainController::QuitGame()
{
	ConsoleCommand("quit");
}

void AMainController::SwitchLocker()
{
	CurLocker->Switch();
}

bool AMainController::DoesCursorInMenu()
{
	bool bTriggerMenu;

	FVector2D ViewportSize, MousePosition;
	GetLocalPlayer()->ViewportClient->GetViewportSize(ViewportSize);  //获取客户端窗口大小
	GetMousePosition(MousePosition.X, MousePosition.Y);       //获取鼠标坐标

	bTriggerMenu = (MousePosition.X / ViewportSize.X) < (static_cast<float>(400) / static_cast<float>(2048));
	return bTriggerMenu;
}

void AMainController::LoadLandscape(FName const LevelName)
{
	if (GetWorld())
	{
		UGameplayStatics::LoadStreamLevel(GetWorld(), LevelName, true, false, FLatentActionInfo());
	}
}

void AMainController::OnePointPressed(const FVector2D& Point, float DownTime)
{
	if (GEngine)
	{
		FVector2D ScreenSize;
		GEngine->GameViewport->GetViewportSize(ScreenSize);


	}
}

void AMainController::OnePointReleased(const FVector2D& Point, float DownTime)
{

}

void AMainController::OnePointRepeat(const FVector2D& Point, float DownTime)
{

}
