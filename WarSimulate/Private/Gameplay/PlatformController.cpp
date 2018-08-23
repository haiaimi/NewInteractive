// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/PlatformController.h"
#include "ConstructorHelpers.h"
#include "Table.h"
#include "Engine/Engine.h"
#include "Common/OriginHelper.h"
#include "Engine/LocalPlayer.h"
#include "SceneView.h"
#include "InventoryActor.h"
#include "Locker.h"
#include "WarSimulateProject.h"
#include "Engine/World.h"
#include "UI/WarSimulateHUD.h"
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
#include "SInventoryMenuWidget.h"
#include "Engine/PostProcessVolume.h"
#include "EngineUtils.h"
#include "TCMultiSelectComponent.h"
#include "Components/StaticMeshComponent.h"
#include "FlightPlatform.h"
#include "Engine/GameViewportClient.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"


APlatformController::APlatformController()
	:TargetActor(nullptr),
	CurDragThing(nullptr),
	CurLocker(nullptr),
	CurMenuSpawnThing(nullptr),
	CurTable(nullptr),
	InputHandle(nullptr),
	MaterialIndex(0),
	PinchComponent(nullptr),
	PostProcessVoulme(nullptr),
	bInPreview(false),
	bHasLandscape(false),
	ControlPlatform(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;      //Controller每帧更新
	MaterialInventory.Reset();

	PinchComponent = CreateDefaultSubobject<UTCPinchComponent>(TEXT("PinchComponent"));    //创建Pinch触控操作组件
	TapComponent = CreateDefaultSubobject<UTCTapComponent>(TEXT("TapComponent"));
	SwitchUIComponent = CreateDefaultSubobject<UTCSwitchUIComponent>(TEXT("SwitchUIComponent"));
	DragSwipeComponent = CreateDefaultSubobject<UTCDragSwipeComponent>(TEXT("DragSwipeComponent"));
	MultiSelectComponent = CreateDefaultSubobject<UTCMultiSelectComponent>(TEXT("MultiSelectComponent"));

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

void APlatformController::BeginPlay()
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
				BIND_1P_ACTION(InputHandle,EGameTouchKey::Hold,IE_Pressed,this,&APlatformController::OnHoldPressed)
				BIND_1P_ACTION(InputHandle, EGameTouchKey::Swipe, IE_Pressed, CurLocker, &ALocker::StartOpenLocker);
				BIND_1P_ACTION(InputHandle, EGameTouchKey::Swipe, IE_Released, CurLocker, &ALocker::EndOpenLocker);
				BIND_1P_ACTION(InputHandle, EGameTouchKey::Swipe, IE_Pressed,this, &APlatformController::OnSwipePressed);
				BIND_1P_ACTION(InputHandle, EGameTouchKey::Swipe, IE_Released, this, &APlatformController::OnSwipeReleased);
				BIND_1P_ACTION(InputHandle, EGameTouchKey::Swipe, IE_Repeat, this, &APlatformController::OnSwipeUpdate);
				BIND_1P_ACTION(InputHandle, EGameTouchKey::Tap, IE_Pressed, this, &APlatformController::TapPressed);
				BIND_1P_ACTION(InputHandle, EGameTouchKey::DoubleTap, IE_Pressed, this, &APlatformController::DoubleTapPressed);
				BIND_2P_ACTION(InputHandle, EGameTouchKey::Pinch, IE_Pressed, this, &APlatformController::OnPinchStart);
				BIND_2P_ACTION(InputHandle, EGameTouchKey::Pinch, IE_Released, this, &APlatformController::OnPinchEnd);
				BIND_2P_ACTION(InputHandle, EGameTouchKey::Pinch, IE_Repeat, this, &APlatformController::OnPinchUpdate);
				
				if (SwitchUIComponent)
				{
					BIND_2MP_ACTION(InputHandle, EGameTouchKey::FivePoints, IE_Pressed, SwitchUIComponent, &UTCSwitchUIComponent::OnFivePointsPressed);
					BIND_2MP_ACTION(InputHandle, EGameTouchKey::FivePoints, IE_Released, SwitchUIComponent, &UTCSwitchUIComponent::OnFivePointsReleased);
					BIND_2MP_ACTION(InputHandle, EGameTouchKey::FivePoints, IE_Repeat, SwitchUIComponent, &UTCSwitchUIComponent::OnFivePointsUpdate);
					SwitchUIComponent->ReleasedEventOpen.BindUObject(this, &APlatformController::OpenUI);
					SwitchUIComponent->ReleasedEventClose.BindUObject(this, &APlatformController::CloseUI);
				}
			}
		}
	});
	GetWorldTimerManager().SetTimer(SpawnLockerHandle, TimerDelegate, 0.1f, false);

	for (TActorIterator<APostProcessVolume> Iter(GetWorld()); Iter; ++Iter)
	{
		PostProcessVoulme = *Iter;
	}
}

void APlatformController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	/*if (CurLocker->WasRecentlyRendered())
		TestProjectHelper::Debug_ScreenMessage(FString::Printf(TEXT("Last Render Time: %f"), CurLocker->GetLastRenderTime()));*/
}

void APlatformController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindAction(TEXT("ToggleMaterial"), EInputEvent::IE_Pressed, this, &APlatformController::ToggleTableMaterial);
		//InputComponent->BindAction(TEXT("Drag"), EInputEvent::IE_Pressed, this, &AMainController::DragSomeThing);
		//InputComponent->BindAction(TEXT("Drag"), EInputEvent::IE_Released, this, &AMainController::StopDrag);
		InputComponent->BindAction(TEXT("Quit"), EInputEvent::IE_Pressed, this, &APlatformController::QuitGame);
		InputComponent->BindAction(TEXT("LockerSwitch"), EInputEvent::IE_Pressed, this, &APlatformController::SwitchLocker);
		InputComponent->BindAction(TEXT("ToggleTarget"), EInputEvent::IE_Pressed, this, &APlatformController::ToggleTarget);
		InputComponent->BindAction(TEXT("ScreenShot"), EInputEvent::IE_Pressed, this, &APlatformController::ScreenShot);
		//InputComponent->BindAction(TEXT("PopMenu"), EInputEvent::IE_Pressed, this, &AMainController::SpawnNewWidget);
	}
}

void APlatformController::ProcessPlayerInput(const float DeltaTime, const bool bGamePaused)
{
	if (InputHandle && PlayerInput)
		InputHandle->UpdateInputStates(DeltaTime);

	Super::ProcessPlayerInput(DeltaTime, bGamePaused);
}

void APlatformController::ToggleTableMaterial()
{
	if (CurTable)
	{
		MaterialIndex = (MaterialIndex + 1) % 3;
		CurTable->ToggleMaterial(MaterialInventory[MaterialIndex]);
	}
}

void APlatformController::DragSomeThing()
{
	FVector WorldPosition, WorldDirection;
	OriginHelper::DeprojectScreenToWorld_Cursor(this, WorldPosition, WorldDirection);

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

void APlatformController::StopDrag(FVector2D StopPoint)
{
	if (CurDragThing)
	{
		//检测是否停留在储物柜上，然后决定存放位置
		FHitResult HitResult;
		//FCollisionQueryParams QueryParam;
		//QueryParam.bIgnoreBlocks = true;  //忽略阻挡
		//GetWorld()->LineTraceSingleByChannel(HitResult, WorldPosition, WorldPosition + WorldDirection * 1000.f, COLLISION_LOCKERRAY);

		GetHitResultAtScreenPosition(StopPoint, COLLISION_LOCKERRAY, FCollisionQueryParams(), HitResult);
		
		if (ALocker* AimLocker = Cast<ALocker>(HitResult.GetActor()))
		{
			if (CurDragThing->bInLocker)
			{
				AimLocker->RemoveInventoryThing(CurDragThing);        //
				AimLocker->AddInventoryThing(CurDragThing, HitResult.ImpactPoint);
			}
			else if(!CurDragThing->bInLocker && !bInPreview)
			{
				AimLocker->AddInventoryThing(CurDragThing, HitResult.ImpactPoint);
			}
		}
		else if (CurDragThing->bInLocker)
		{
			ALocker* OwnerLocker = Cast<ALocker>(CurDragThing->GetAttachParentActor());
			CurDragThing->bInLocker = false;
			CurDragThing->OriginLocation = CurDragThing->GetActorLocation();  //设置物体当前新的停留位置

			if (OwnerLocker)
			{
				OwnerLocker->RemoveInventoryThing(CurDragThing);
				OwnerLocker->StopCastLight();
			}
		}

		if (AWarSimulateHUD* CurHUD = Cast<AWarSimulateHUD>(GetHUD()))
		{
			if (CurHUD->IsInventoryWidgetValid())
			{
				const bool bInMenu = DoesPointInMenu(StopPoint);
				if (bInMenu && CurHUD->IsMenuShow() && CurMenuSpawnThing)      //菜单正在显示，并且鼠标指针在菜单内
				{
					OriginHelper::Debug_ScreenMessage(TEXT("Stop Drag"));
					CurMenuSpawnThing->Destroy();
					bShouldSpawnActor = false;
				}
			}
		}
		//FSlateApplication::Get().SetCursorPos(FVector2D(0.f, 0.f));       //在触屏状态下恢复鼠标位置
		//CurDragThing->StopMoveWithCursor();
		
		if (!bInPreview)
		{
			CurMenuSpawnThing = nullptr;
			CurDragThing = nullptr;
		}
	}
}

void APlatformController::SpawnInventoryActors(UClass* SpawnedActor)
{
	SpawnActor = SpawnedActor;
	bShouldSpawnActor = true;
}

void APlatformController::QuitGame()
{
	ConsoleCommand("quit");
}

void APlatformController::SwitchLocker()
{
	CurLocker->Switch();
}

bool APlatformController::DoesPointInMenu(FVector2D Point)
{
	bool bTriggerMenu;

	FVector2D ViewportSize;
	GetLocalPlayer()->ViewportClient->GetViewportSize(ViewportSize);  //获取客户端窗口大小

	bTriggerMenu = (Point.X / ViewportSize.X) < (static_cast<float>(400) / static_cast<float>(1920));
	return bTriggerMenu;
}

void APlatformController::LoadLandscape(FName const LevelName)
{
	if (GetWorld())
	{
		UGameplayStatics::LoadStreamLevel(GetWorld(), LevelName, true, false, FLatentActionInfo());
		bHasLandscape = true;
	}
}

void APlatformController::OnHoldPressed(const FVector2D& Point, float DownTime)
{
	if (CurMenuSpawnThing)return;
	AInventoryActor* CurTargetActor = nullptr;
	if (DragSwipeComponent && DragSwipeComponent->IsDragActor())
	{
		CurTargetActor = Cast<AInventoryActor>(DragSwipeComponent->TargetActor);
	}

	FPopMenuInfo NewMenuInfo;
	if (CurTargetActor)
	{
		NewMenuInfo = CurTargetActor->InfoInMenu;
	}
	

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

void APlatformController::OnSwipePressed(const FVector2D& Point, float DownTime)
{
	if (GetGroundCamera() != nullptr && bHasLandscape)
	{
		GetGroundCamera()->StartSwipe(Point, DownTime);
	}

	if (DragSwipeComponent)
	{
		FVector LookDir = ((const UGroundCameraComponent*)UGroundCameraComponent::StaticClass()->GetDefaultObject())->LookRotation.Vector();
		
		if (GetWorld() && bShouldSpawnActor && !bInPreview)      //如果当前已经控制着一个就不需要生成
		{
			FVector WorldPos, WorldDir;
			DeprojectScreenPositionToWorld(Point.X, Point.Y, WorldPos, WorldDir);

			FActorSpawnParameters SpawnParameter;
			SpawnParameter.Owner = this;
			const FTransform ActorSpawnTransform = FTransform(FRotator::ZeroRotator, WorldPos + 800.f*WorldDir);
			CurMenuSpawnThing = Cast<AInventoryActor>(GetWorld()->SpawnActor(SpawnActor, &ActorSpawnTransform, SpawnParameter));
			CurMenuSpawnThing->AttachToActor(CurLocker, FAttachmentTransformRules::KeepWorldTransform);
			CurMenuSpawnThing->AddToCluster(this);  

			if (CurMenuSpawnThing)
			{
				bShouldSpawnActor = false;
				TargetActor = CurMenuSpawnThing;
				CurDragThing = CurMenuSpawnThing;
			}
		}

		DragSwipeComponent->OnDragPressed(Point, LookDir, CurDragThing, MultiSelectedActors);
		TargetActor = DragSwipeComponent->TargetActor;
		CurDragThing = Cast<AInventoryActor>(TargetActor);
	}

	if (MultiSelectComponent && !DragSwipeComponent->IsDragActor() && !bInPreview)
	{
		MultiSelectComponent->OnMultiSelectPressed(Point, DownTime);
	}

	if (TapComponent)
	{
		TapComponent->OnRotateTapPressed(Point, DownTime, TargetActor);
	}
}

void APlatformController::OnSwipeReleased(const FVector2D& Point, float DownTime)
{
	if (GetGroundCamera() != nullptr && bHasLandscape)
	{
		GetGroundCamera()->EndSwipe(Point, DownTime);
	}

	if (TapComponent)
	{
		TapComponent->OnRotateTapReleased(Point, DownTime);
	}

	if (DragSwipeComponent)
	{
		DragSwipeComponent->OnDragReleased(Point);
		StopDrag(Point);
	}

	if (MultiSelectComponent && !CurDragThing)
	{
		MultiSelectComponent->OnMultiSelectReleased(Point, DownTime, MultiSelectedActors);
	}
}

void APlatformController::OnSwipeUpdate(const FVector2D& Point, float DownTime)
{
	/*if (bShouldSpawnActor && !bInPreview)
		OnSwipePressed(Point, DownTime);*/

	if (TapComponent)
	{
		TapComponent->OnRotateTapUpdated(Point, DownTime);
	}

	if (bInPreview)return;

	if (GetGroundCamera() != nullptr && bHasLandscape)
	{
		GetGroundCamera()->UpdateSwipe(Point, DownTime);
	}

	if (DragSwipeComponent && CurDragThing)
	{
		FVector LookDir = ((const UGroundCameraComponent*)UGroundCameraComponent::StaticClass()->GetDefaultObject())->LookRotation.Vector();
		DragSwipeComponent->OnDragUpdate(Point, LookDir, MultiSelectedActors);
	}

	if (MultiSelectComponent && !CurDragThing)
	{
		MultiSelectComponent->OnMultiSelectUpdate(Point, DownTime);
	}
}

void APlatformController::TapPressed(const FVector2D& Point, float DownTime)
{
	OriginHelper::Debug_ScreenMessage(Point.ToString());

	if (PopMenu.IsValid() &&! PopMenu->IsInteractable())    //检测是否有交互，如果没有交互就删除弹出的菜单
	{
		PopMenu.Reset();
	}
}

void APlatformController::DoubleTapPressed(const FVector2D& Point, float DownTime)
{
	OriginHelper::Debug_ScreenMessage(TEXT("Double Tap"));
}

void APlatformController::OnPinchStart(const FVector2D& Point1, const FVector2D& Point2, float DownTime)
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

void APlatformController::OnPinchUpdate(const FVector2D& Point1, const FVector2D& Point2, float DownTime)
{
	if (GetGroundCamera() != nullptr)
	{
		GetGroundCamera()->OnPinchUpdate(InputHandle, Point1, Point2, DownTime);
	}

	if (PinchComponent != nullptr && TargetActor != nullptr)
	{
		PinchComponent->OnPinchUpdated(TargetActor, Point1, Point2);
		/*如果有需要可以在这里更新摄像机的方向*/

		if (AWarSimulateHUD* MyHUD = Cast<AWarSimulateHUD>(GetHUD()))
		{
			MyHUD->DrawCustomDebugLine(true, Point1, Point2);
		}
	}
}

void APlatformController::OnPinchEnd(const FVector2D& Point1, const FVector2D& Point2, float DownTime)
{
	if (GetGroundCamera() != nullptr)
	{
		GetGroundCamera()->OnPinchEnd(Point1, Point2, DownTime);
		OriginHelper::Debug_ScreenMessage(TEXT("Pinch End"));
	}

	if (PinchComponent != nullptr && TargetActor != nullptr)
	{
		PinchComponent->OnPinchReleased(TargetActor, Point1, Point2);

		if (AWarSimulateHUD* MyHUD = Cast<AWarSimulateHUD>(GetHUD()))
		{
			MyHUD->DrawCustomDebugLine(false, Point1, Point2);
		}
	}
}

void APlatformController::OpenUI()
{
	SwitchUI(true);
}

void APlatformController::CloseUI()
{
	SwitchUI(false);
}

void APlatformController::SwitchUI(bool bOpenUI)
{
	if (AWarSimulateHUD* CurHUD = Cast<AWarSimulateHUD>(GetHUD()))
	{
		if (CurHUD->IsInventoryWidgetValid())
		{
			CurHUD->ShowMenu(bOpenUI);
		}
	}
}

bool APlatformController::CanDragLanscape()
{
	bool bCanDrag = true;
	// 当用户在拖拽物体时
	if (CurDragThing)
		bCanDrag = false;

	//用户在UI内时
	if (AWarSimulateHUD* CurHUD = Cast<AWarSimulateHUD>(GetHUD()))
	{
		if (CurHUD->IsInventoryWidgetValid())
		{
			/*if (DoesCursorInMenu() && CurHUD->IsMenuShow())
				bCanDrag = false;*/
		}
	}

	return bCanDrag;
}

void APlatformController::ShowHighlight(bool bShow)
{
	if (PostProcessVoulme)
		PostProcessVoulme->bEnabled = bShow;
}

class AGroundSpectatorPawn* APlatformController::GetGroundSpectatorPawn() const
{
	return Cast<AGroundSpectatorPawn>(GetPawn());
}

class UGroundCameraComponent* APlatformController::GetGroundCamera()
{
	UGroundCameraComponent* CameraComponent = nullptr;
	if (GetGroundSpectatorPawn() != nullptr)
		CameraComponent = GetGroundSpectatorPawn()->GetGroundCameraComponent();

	return CameraComponent;
}

void APlatformController::StartPreview()
{
	if (CurMenuSpawnThing && !bInPreview)   //当前欲生成的物体存在
	{
		bInPreview = true;
		CurDragThing = nullptr;
		CurMenuSpawnThing->AttachToActor(GetPawn(), FAttachmentTransformRules::KeepWorldTransform);
		CurMenuSpawnThing->RemoveTouchTypes_Implementation(ECustomTouchType::AllTouchType);
		CurMenuSpawnThing->AddTouchTypes_Implementation(ECustomTouchType::RotateSwipe_1P);

		const FVector ViewLocation = GetFocalLocation();
		FRotator LookRotation = ((const UGroundCameraComponent*)UGroundCameraComponent::StaticClass()->GetDefaultObject())->LookRotation;
		const FVector RelDestLoc = LookRotation.Vector()*200.f;
		CurMenuSpawnThing->BeginMove(RelDestLoc);

		GetGroundCamera()->BlurMode(true);
	}
}

void APlatformController::StopPreview()
{
	if (bInPreview)
	{
		bInPreview = false;
		CurMenuSpawnThing->Destroy();
		CurMenuSpawnThing = nullptr;
		CurDragThing = nullptr;

		GetGroundCamera()->BlurMode(false);
	}
}

void APlatformController::SetNextControlPlatform(class AFlightPlatform* InPlatformRef)
{
	ControlPlatform = InPlatformRef;
}

void APlatformController::ToggleTarget()
{
	/*for (TActorIterator<AFlightPlatform> It(GetWorld()); It; ++It)
	{
		if (*It != ControlPlatform)
		{
			SetViewTargetWithBlend(*It, 1.f, EViewTargetBlendFunction::VTBlend_Linear);
			FTimerDelegate Delegate;
			Delegate.BindUObject(this, &APlatformController::PossessNewTarget);
			GetWorldTimerManager().SetTimer(PossessHandle, Delegate, 1.f, false);
			ControlPlatform = *It;
			break;
		}
	}*/

	if (ControlPlatform)
	{
		SetViewTargetWithBlend(ControlPlatform, 1.f, EViewTargetBlendFunction::VTBlend_Linear);
		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &APlatformController::PossessNewTarget);
		GetWorldTimerManager().SetTimer(PossessHandle, Delegate, 1.f, false);
	}
}

void APlatformController::PossessNewTarget()
{
	static bool bControlPlatform = true;
	if (AActor* TargetActor = GetViewTarget())
	{
		AInventoryActor* TempTarget = Cast<AInventoryActor>(TargetActor);
		/*if (bControlPlatform)
		{
			bControlPlatform = false;
			TempTarget->PlatformData->PlatformPos = FVector(-225.f, 0.f, 17454.f);
			TempTarget->PlatformData->bControlled = true;
		}*/

		Possess(TempTarget);
	}
}

void APlatformController::ScreenShot()
{
	//从Viewport获取截屏，只能获取渲染的画面
	FViewport* viewport = GetLocalPlayer()->ViewportClient->Viewport;
	//FViewport* viewport = GetWorld()->GetGameViewport()->Viewport;
	//viewport->ReadPixels(PixelBuffer);

	AWarSimulateHUD* CurHUD = Cast<AWarSimulateHUD>(GetHUD());
	if (CurHUD)
	{
		if (!CurHUD->IsInventoryWidgetValid())
			return;
	}

	TArray<FColor> PixelBuffer;
	FIntVector Res;

	TSharedPtr<SInventoryMenuWidget>& Widget = CurHUD->GetMenuWidget();
	FSlateApplication::Get().TakeScreenshot(Widget.ToSharedRef(), PixelBuffer, Res);

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	ImageWrapper->SetRaw((void*)PixelBuffer.GetData(), PixelBuffer.GetAllocatedSize(), viewport->GetSizeXY().X, viewport->GetSizeXY().Y, ERGBFormat::BGRA, 8);      //设置JPEG格式图片数据

	if (ImageWrapper.IsValid())       //
	{
		TArray<uint8> ImageData = ImageWrapper->GetCompressed();
		FString ScreenShotDir = FPaths::ScreenShotDir() / TEXT("ScreenShot");
		IFileManager::Get().MakeDirectory(*FPaths::ScreenShotDir(), true);
		FString SavePath = ScreenShotDir + TEXT("000.jpg");
		int32 PictureIndex = 1;      //截图文件夹中图片序号
		while (IFileManager::Get().FileExists(*SavePath))
		{
			SavePath = ScreenShotDir;

			if (PictureIndex <= 99)
				SavePath.Append(TEXT("0"));
			if (PictureIndex <= 9)
				SavePath.Append(TEXT("0"));
			SavePath.AppendInt(PictureIndex);
			SavePath.Append(".jpg");

			PictureIndex++;
		}
		FFileHelper::SaveArrayToFile(ImageData, *SavePath);            //保存图片数据
	}
}