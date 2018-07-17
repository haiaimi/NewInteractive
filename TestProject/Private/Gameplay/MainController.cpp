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
#include "GroundSpectatorPawn.h"
#include "TCPinchComponent.h"
#include "TCTapComponent.h"
#include "TCSwitchUIComponent.h"
#include "TCDragSwipeComponent.h"
#include "GameFramework/PlayerInput.h"
#include "SPopMenuWidget.h"


AMainController::AMainController()
	:TargetActor(nullptr),
	CurDragThing(nullptr),
	CurLocker(nullptr),
	CurMenuSpawnThing(nullptr),
	CurTable(nullptr),
	InputHandle(nullptr),
	MaterialIndex(0),
	PinchComponent(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;      //Controller每帧更新
	MaterialInventory.Reset();

	PinchComponent = CreateDefaultSubobject<UTCPinchComponent>(TEXT("PinchComponent"));    //创建Pinch触控操作组件
	TapComponent = CreateDefaultSubobject<UTCTapComponent>(TEXT("TapComponent"));
	SwitchUIComponent = CreateDefaultSubobject<UTCSwitchUIComponent>(TEXT("SwitchUIComponent"));
	DragSwipeComponent = CreateDefaultSubobject<UTCDragSwipeComponent>(TEXT("DragSwipeComponent"));

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material_1(TEXT("/Game/StarterContent/Materials/M_Wood_Pine"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material_2(TEXT("/Game/StarterContent/Materials/M_Wood_Oak"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material_3(TEXT("/Game/StarterContent/Materials/M_Wood_Walnut"));

	if (Material_1.Succeeded())MaterialInventory.Add(Material_1.Object);
	if (Material_2.Succeeded())MaterialInventory.Add(Material_2.Object);
	if (Material_3.Succeeded())MaterialInventory.Add(Material_3.Object);

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

	const FVector TableSapwnLoaction = ViewLocation + TableX2D * -30.f + LookRotation.Vector() * 550.f;
	const FVector LockerSpawnLocation = ViewLocation + TableX2D * 170.f + LookRotation.Vector() * 530.f;
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;

	//ATable* SpawnedTable = GetWorld()->SpawnActor<ATable>(TableSapwnLoaction, FRotator::ZeroRotator, SpawnParam);
	ALocker* SpawnedLocker = GetWorld()->SpawnActor<ALocker>(LockerSpawnLocation, FRotator::ZeroRotator, SpawnParam);

	//if (SpawnedTable)
	//{
	//	CurTable = SpawnedTable;
	//	if (GetPawn())
	//		CurTable->AttachToActor(GetPawn(), FAttachmentTransformRules::KeepWorldTransform);       //绑定到Pawn中
	//}

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
				BIND_1P_ACTION(InputHandle,EGameTouchKey::Hold,IE_Pressed,this,&AMainController::OnHoldPressed)
				BIND_1P_ACTION(InputHandle, EGameTouchKey::Swipe, IE_Pressed, CurLocker, &ALocker::StartOpenLocker);
				BIND_1P_ACTION(InputHandle, EGameTouchKey::Swipe, IE_Released, CurLocker, &ALocker::EndOpenLocker);
				BIND_1P_ACTION(InputHandle, EGameTouchKey::Swipe, IE_Pressed,this, &AMainController::OnSwipePressed);
				BIND_1P_ACTION(InputHandle, EGameTouchKey::Swipe, IE_Released, this, &AMainController::OnSwipeReleased);
				BIND_1P_ACTION(InputHandle, EGameTouchKey::Swipe, IE_Repeat, this, &AMainController::OnSwipeUpdate);
				BIND_1P_ACTION(InputHandle, EGameTouchKey::Tap, IE_Pressed, this, &AMainController::TapPressed);
				BIND_1P_ACTION(InputHandle, EGameTouchKey::DoubleTap, IE_Pressed, this, &AMainController::DoubleTapPressed);
				BIND_2P_ACTION(InputHandle, EGameTouchKey::Pinch, IE_Pressed, this, &AMainController::OnPinchStart);
				BIND_2P_ACTION(InputHandle, EGameTouchKey::Pinch, IE_Released, this, &AMainController::OnPinchEnd);
				BIND_2P_ACTION(InputHandle, EGameTouchKey::Pinch, IE_Repeat, this, &AMainController::OnPinchUpdate);
				
				if (SwitchUIComponent)
				{
					BIND_2MP_ACTION(InputHandle, EGameTouchKey::FivePoints, IE_Pressed, SwitchUIComponent, &UTCSwitchUIComponent::OnFivePointsPressed);
					BIND_2MP_ACTION(InputHandle, EGameTouchKey::FivePoints, IE_Released, SwitchUIComponent, &UTCSwitchUIComponent::OnFivePointsReleased);
					BIND_2MP_ACTION(InputHandle, EGameTouchKey::FivePoints, IE_Repeat, SwitchUIComponent, &UTCSwitchUIComponent::OnFivePointsUpdate);
					SwitchUIComponent->ReleasedEventOpen.BindUObject(this, &AMainController::OpenUI);
					SwitchUIComponent->ReleasedEventClose.BindUObject(this, &AMainController::CloseUI);
				}
			}
		}
	});
	GetWorldTimerManager().SetTimer(SpawnLockerHandle, TimerDelegate, 0.1f, false);
}

void AMainController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AMainController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindAction(TEXT("ToggleMaterial"), EInputEvent::IE_Pressed, this, &AMainController::ToggleTableMaterial);
		//InputComponent->BindAction(TEXT("Drag"), EInputEvent::IE_Pressed, this, &AMainController::DragSomeThing);
		//InputComponent->BindAction(TEXT("Drag"), EInputEvent::IE_Released, this, &AMainController::StopDrag);
		InputComponent->BindAction(TEXT("Quit"), EInputEvent::IE_Pressed, this, &AMainController::QuitGame);
		InputComponent->BindAction(TEXT("LockerSwitch"), EInputEvent::IE_Pressed, this, &AMainController::SwitchLocker);
		//InputComponent->BindAction(TEXT("PopMenu"), EInputEvent::IE_Pressed, this, &AMainController::SpawnNewWidget);
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
		if (HitActor)
		{
			CurDragThing = HitActor;
			TargetActor = CurDragThing;
			const FVector Offset = CurDragThing->GetActorLocation() - HitResult.ImpactPoint;   //鼠标指针相对于物体的位置
			const FPlane MovePlane(HitResult.ImpactPoint, FRotationMatrix(CurDragThing->GetActorRotation()).GetUnitAxis(EAxis::Z));   //获取鼠标与物体撞击点的平面
			//CurDragThing->StartMoveWithCursor(this, Offset, MovePlane);
		}
	}

	//if (ATestProjectHUD* CurHUD = Cast<ATestProjectHUD>(GetHUD()))
	//{
	//	if (CurHUD->IsInventoryWidgetValid())
	//	{
	//		const bool bTriggerMenu = DoesCursorInMenu();
	//		if (bTriggerMenu)
	//		{
	//			CurHUD->ShowMenu(true);      //显示菜单
	//		}
	//		else if(!bTriggerMenu && !HitActor)
	//		{
	//			CurHUD->ShowMenu(false);       //可能会关闭菜单
	//		}
	//	}
	//}
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
				AimLocker->RemoveInventoryThing(CurDragThing);        //
				AimLocker->AddInventoryThing(CurDragThing, HitResult.ImpactPoint);
			}
			else
			{
				//TestProjectHelper::Debug_ScreenMessage(TEXT("Catch Locker"));
				AimLocker->AddInventoryThing(CurDragThing, HitResult.ImpactPoint);
			}
		}
		else
		{
			//if (ATestProjectHUD* CurHUD = Cast<ATestProjectHUD>(GetHUD()))
			//{
			//	if (CurHUD->IsInventoryWidgetValid())
			//	{
			//		const bool bInMenu = DoesCursorInMenu();
			//		if (bInMenu && CurHUD->IsMenuShow() && CurMenuSpawnThing)      //菜单正在显示，并且鼠标指针在菜单内
			//		{
			//			CurMenuSpawnThing->Destroy();
			//		}
			//		else
			//		{
			//			CurDragThing->OriginLocation = CurDragThing->GetActorLocation();  //设置物体当前新的停留位置
			//			if (CurDragThing->bInLocker)
			//			{
			//				ALocker* OwnerLocker = Cast<ALocker>(CurDragThing->GetAttachParentActor());
			//				CurDragThing->bInLocker = false;

			//				if (OwnerLocker)
			//				{
			//					OwnerLocker->RemoveInventoryThing(CurDragThing);
			//					OwnerLocker->StopCastLight();
			//				}
			//				//CurDragThing->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			//			}
			//		}
			//	}
			//}
		}
		FSlateApplication::Get().SetCursorPos(FVector2D(0.f, 0.f));       //在触屏状态下恢复鼠标位置
		CurDragThing->StopMoveWithCursor();
		CurDragThing = nullptr;
		CurMenuSpawnThing = nullptr;
	}
}

void AMainController::SpawnInventoryActors(UClass* SpawnedActor)
{
	SpawnActor = SpawnedActor;
	bShouldSpawnActor = true;
}

void AMainController::QuitGame()
{
	ConsoleCommand("quit");
}

void AMainController::SwitchLocker()
{
	CurLocker->Switch();
}

bool AMainController::DoesPointInMenu(FVector2D Point)
{
	bool bTriggerMenu;

	FVector2D ViewportSize;
	GetLocalPlayer()->ViewportClient->GetViewportSize(ViewportSize);  //获取客户端窗口大小

	bTriggerMenu = (Point.X / ViewportSize.X) < (static_cast<float>(400) / static_cast<float>(1920));
	return bTriggerMenu;
}

void AMainController::LoadLandscape(FName const LevelName)
{
	if (GetWorld())
	{
		UGameplayStatics::LoadStreamLevel(GetWorld(), LevelName, true, false, FLatentActionInfo());
	}
}

void AMainController::OnHoldPressed(const FVector2D& Point, float DownTime)
{
	TestProjectHelper::Debug_ScreenMessage(Point.ToString());

	FPopMenuInfo NewMenuInfo;
	NewMenuInfo.ButtonsNum = 8;

	FVector2D ScreenSize;
	GetLocalPlayer()->ViewportClient->GetViewportSize(ScreenSize);

	const FVector2D StandardPos(Point.X*1920.f / ScreenSize.X, Point.Y*1080.f / ScreenSize.Y);       //在标准分辨率中的位置
	const FVector2D NoAffectArea(1920.f - PopMenuWidth, 1080.f - NewMenuInfo.ButtonsNum*PopButtonHeight - 40.f);    //PopMenu生成不受影响的区域

	//下面就是对各个情况的判断
	if (StandardPos.X <= NoAffectArea.X && StandardPos.Y <= NoAffectArea.Y)
	{
		NewMenuInfo.MenuPos = FMargin(StandardPos.X, StandardPos.Y, 0.f, 0.f);
		NewMenuInfo.HorizontalAlignType = EHorizontalAlignment::HAlign_Left;
	}
	else if (StandardPos.X > NoAffectArea.X && StandardPos.Y <= NoAffectArea.Y)
	{
		NewMenuInfo.MenuPos = FMargin(0.f, StandardPos.Y, 1920.f - StandardPos.X, 0.f);
		NewMenuInfo.HorizontalAlignType = EHorizontalAlignment::HAlign_Right;
	}
	else if (StandardPos.X <= NoAffectArea.X && StandardPos.Y > NoAffectArea.Y)
	{
		NewMenuInfo.MenuPos = FMargin(StandardPos.X, NoAffectArea.Y, 0.f, 0.f);
		NewMenuInfo.HorizontalAlignType = EHorizontalAlignment::HAlign_Left;
	}
	else
	{
		NewMenuInfo.MenuPos = FMargin(0.f, NoAffectArea.Y, 1920.f - StandardPos.X, 0.f);
		NewMenuInfo.HorizontalAlignType = EHorizontalAlignment::HAlign_Right;
	}

	//创建并显示控件
	SAssignNew(PopMenu, SPopMenuWidget)
		.InMenuInfo(NewMenuInfo);

	if (PopMenu.IsValid())
	{
		GEngine->GameViewport->AddViewportWidgetContent(
			SNew(SWeakWidget).
			PossiblyNullContent(PopMenu.ToSharedRef()), 
			0
			);
	}
}

void AMainController::OnSwipePressed(const FVector2D& Point, float DownTime)
{
	if (GetGroundCamera() != nullptr)
	{
		//GetGroundCamera()->StartSwipe(Point, DownTime);
	}

	if (TapComponent)
	{
		TapComponent->OnRotateTapPressed(Point, DownTime);
	}

	if (DragSwipeComponent)
	{
		FVector LookDir = ((const UGroundCameraComponent*)UGroundCameraComponent::StaticClass()->GetDefaultObject())->LookRotation.Vector();
		DragSwipeComponent->OnDragPressed(Point, LookDir);
		
		if (GetWorld() && bShouldSpawnActor)      //如果当前已经控制着一个就不需要生成
		{
			FVector WorldPos, WorldDir;
			DeprojectScreenPositionToWorld(Point.X, Point.Y, WorldPos, WorldDir);

			FTransform ActorSpawnTransform = FTransform(FRotator::ZeroRotator, WorldPos + 600.f*WorldDir);
			CurMenuSpawnThing = Cast<AInventoryActor>(GetWorld()->SpawnActor(SpawnActor, &ActorSpawnTransform));
			CurMenuSpawnThing->AttachToActor(CurLocker, FAttachmentTransformRules::KeepWorldTransform);

			bShouldSpawnActor = false;
			TargetActor = CurMenuSpawnThing;
			CurDragThing = CurMenuSpawnThing;
		}
	}
}

void AMainController::OnSwipeReleased(const FVector2D& Point, float DownTime)
{
	if (GetGroundCamera() != nullptr)
	{
		//GetGroundCamera()->EndSwipe(Point, DownTime);
	}

	if (TapComponent)
	{
		TapComponent->OnRotateTapReleased(Point, DownTime);
	}

	if (DragSwipeComponent)
	{
		DragSwipeComponent->OnDragReleased(Point);
	}
}

void AMainController::OnSwipeUpdate(const FVector2D& Point, float DownTime)
{
	if (GetGroundCamera() != nullptr)
	{
		//GetGroundCamera()->UpdateSwipe(Point, DownTime);
	}

	if (TapComponent)
	{
		TapComponent->OnRotateTapUpdated(Point, DownTime);
	}

	if (DragSwipeComponent)
	{
		FVector LookDir = ((const UGroundCameraComponent*)UGroundCameraComponent::StaticClass()->GetDefaultObject())->LookRotation.Vector();
		DragSwipeComponent->OnDragUpdate(Point, LookDir);
	}
}

void AMainController::TapPressed(const FVector2D& Point, float DownTime)
{
	TestProjectHelper::Debug_ScreenMessage(Point.ToString());

	if (PopMenu.IsValid() &&! PopMenu->IsInteractable())    //检测是否有交互，如果没有交互就删除弹出的菜单
	{
		PopMenu.Reset();
	}
}

void AMainController::DoubleTapPressed(const FVector2D& Point, float DownTime)
{
	TestProjectHelper::Debug_ScreenMessage(TEXT("Double Tap"));
}

void AMainController::OnPinchStart(const FVector2D& Point1, const FVector2D& Point2, float DownTime)
{
	if (GetGroundCamera() != nullptr)
	{
		GetGroundCamera()->OnPinchStart(Point1, Point2, DownTime);
	}

	if (PinchComponent != nullptr && TargetActor != nullptr)
	{
		FRotator LookRotation = ((const UGroundCameraComponent*)UGroundCameraComponent::StaticClass()->GetDefaultObject())->LookRotation;
		PinchComponent->SetCameraDirection(LookRotation); //设置摄像机的方向
		PinchComponent->OnPinchPressed(TargetActor, Point1, Point2);
	}
}

void AMainController::OnPinchUpdate(const FVector2D& Point1, const FVector2D& Point2, float DownTime)
{
	if (GetGroundCamera() != nullptr)
	{
		//GetGroundCamera()->OnPinchUpdate(InputHandle, Point1, Point2, DownTime);
	}

	if (PinchComponent != nullptr && TargetActor != nullptr)
	{
		PinchComponent->OnPinchUpdated(TargetActor, Point1, Point2);
		/*如果有需要可以在这里更新摄像机的方向*/

		if (ATestProjectHUD* MyHUD = Cast<ATestProjectHUD>(GetHUD()))
		{
			MyHUD->DrawCustomDebugLine(true, Point1, Point2);
		}
	}
}

void AMainController::OnPinchEnd(const FVector2D& Point1, const FVector2D& Point2, float DownTime)
{
	if (GetGroundCamera() != nullptr)
	{
		GetGroundCamera()->OnPinchEnd(Point1, Point2, DownTime);
	}

	if (PinchComponent != nullptr && TargetActor != nullptr)
	{
		PinchComponent->OnPinchReleased(TargetActor, Point1, Point2);

		if (ATestProjectHUD* MyHUD = Cast<ATestProjectHUD>(GetHUD()))
		{
			MyHUD->DrawCustomDebugLine(false, Point1, Point2);
		}
	}
}

void AMainController::OpenUI()
{
	SwitchUI(true);
}

void AMainController::CloseUI()
{
	SwitchUI(false);
}

void AMainController::SwitchUI(bool bOpenUI)
{
	if (ATestProjectHUD* CurHUD = Cast<ATestProjectHUD>(GetHUD()))
	{
		if (CurHUD->IsInventoryWidgetValid())
		{
			CurHUD->ShowMenu(bOpenUI);
		}
	}
}

bool AMainController::CanDragLanscape()
{
	bool bCanDrag = true;
	// 当用户在拖拽物体时
	if (CurDragThing)
		bCanDrag = false;

	//用户在UI内时
	if (ATestProjectHUD* CurHUD = Cast<ATestProjectHUD>(GetHUD()))
	{
		if (CurHUD->IsInventoryWidgetValid())
		{
			/*if (DoesCursorInMenu() && CurHUD->IsMenuShow())
				bCanDrag = false;*/
		}
	}

	return bCanDrag;
}

void AMainController::SpawnNewWidget()
{
	FPopMenuInfo NewMenuInfo;
	NewMenuInfo.ButtonsNum = 4;

	FVector2D MousePos, ScreenSize;
	GetMousePosition(MousePos.X, MousePos.Y);
	GetLocalPlayer()->ViewportClient->GetViewportSize(ScreenSize);

	const FVector2D StandardPos(MousePos.X*1920.f / ScreenSize.X, MousePos.Y*1080.f / ScreenSize.Y);       //在标准分辨率中的位置
	const FVector2D NoAffectArea(1920.f - PopMenuWidth, 1080.f - NewMenuInfo.ButtonsNum*PopButtonHeight - 40.f);    //PopMenu生成不受影响的区域

	//下面就是对各个情况的判断
	if (StandardPos.X <= NoAffectArea.X && StandardPos.Y <= NoAffectArea.Y)
	{
		NewMenuInfo.MenuPos = FMargin(StandardPos.X, StandardPos.Y, 0.f, 0.f);
		NewMenuInfo.HorizontalAlignType = EHorizontalAlignment::HAlign_Left;
	}
	else if (StandardPos.X > NoAffectArea.X && StandardPos.Y <= NoAffectArea.Y)
	{
		NewMenuInfo.MenuPos = FMargin(0.f, StandardPos.Y, 1920.f - StandardPos.X, 0.f);
		NewMenuInfo.HorizontalAlignType = EHorizontalAlignment::HAlign_Right;
	}
	else if (StandardPos.X <= NoAffectArea.X && StandardPos.Y > NoAffectArea.Y)
	{
		NewMenuInfo.MenuPos = FMargin(StandardPos.X, NoAffectArea.Y, 0.f, 0.f);
		NewMenuInfo.HorizontalAlignType = EHorizontalAlignment::HAlign_Left;
	}
	else
	{
		NewMenuInfo.MenuPos = FMargin(0.f, NoAffectArea.Y, 1920.f - StandardPos.X, 0.f);
		NewMenuInfo.HorizontalAlignType = EHorizontalAlignment::HAlign_Right;
	}

	//创建并显示控件
	SAssignNew(PopMenu, SPopMenuWidget)
		.InMenuInfo(NewMenuInfo);

	if (PopMenu.IsValid())
	{
		GEngine->GameViewport->AddViewportWidgetContent(
			SNew(SWeakWidget).
			PossiblyNullContent(PopMenu.ToSharedRef()), 
			0
			);
	}
}

class AGroundSpectatorPawn* AMainController::GetGroundSpectatorPawn() const
{
	return Cast<AGroundSpectatorPawn>(GetPawn());
}

class UGroundCameraComponent* AMainController::GetGroundCamera() const
{
	UGroundCameraComponent* CameraComponent = nullptr;
	if (GetGroundSpectatorPawn() != nullptr)
		CameraComponent = GetGroundSpectatorPawn()->GetGroundCameraComponent();

	return CameraComponent;
}
