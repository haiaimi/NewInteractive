// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/CustomTouchInterface.h"
#include "SPopMenuWidget.h"
#include "Gameplay/WarSimulateInstance.h"
#include "GameFramework/Pawn.h"
#include "InventoryActor.generated.h"

typedef FDelegateHandle(AInventoryActor::*BindFunctionPtr)(AInventoryActor*);
typedef FDelegateHandle(AInventoryActor::*GetDelegateHandlePtr)();
typedef void(AInventoryActor::*RemoveDelegatePtr)(AInventoryActor*);
extern TMap<FName, BindFunctionPtr> GlobalBindFunctions;     //全局函数指针（绑定对象的函数指针）
extern TMap<FName, RemoveDelegatePtr> GlobalRemoveDelegates;   

struct FBaseActorData;
struct FFlightPlatformData;
class AFlightPlatform;

/**
 * 该类是仓库中物体的基类，只包含基本功能，负责基本的场景显示，其他详细逻辑在子类中
 */
UCLASS()
class WARSIMULATE_API AInventoryActor : public APawn, public ICustomTouchInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInventoryActor(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents()override;

	virtual void BeginDestroy()override;

	/// CustomTouchInterface
	virtual void GetTouchTypes_Implementation(TArray<TEnumAsByte<ECustomTouchType::Type>>& OutTypes)override;

	// Begin CustonTouchInterface
	virtual void AddTouchTypes_Implementation(ECustomTouchType::Type InType)override;

	virtual void RemoveTouchTypes_Implementation(ECustomTouchType::Type InType)override;

	/**为了在蓝图中覆写接口函数*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CanSuitTargetTouchType(ECustomTouchType::Type TargetTouchType, bool& Out);

	virtual void CanSuitTargetTouchType_Implementation(ECustomTouchType::Type TargetTouchType, bool& Out)override;

	virtual void InterfaceFunction(ECustomTouchType::Type InType)override {};
	/// End CustomTouchInterface

public:
	UFUNCTION()
	virtual void BeginCursorOver(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	virtual void EndCursorOver(UPrimitiveComponent* TouchedComponent);

	/**获取物体的高度*/
	virtual float GetHeight();

	/**跟随鼠标移动*/
	void StartMoveWithCursor(class AMainController* Owner, const FVector Offset, const FPlane MovePalne);

	void StopMoveWithCursor();

	/**获取物体相对于父物体的位置*/
	FVector GetRelativeLocation(); 

	UFUNCTION(BlueprintCallable)
	virtual void ShowHighlight(bool bShow);

	FReply DestroySelf();

	virtual void Destroyed()override;

	void BeginMove(const FVector& DestLoc);

	/**切换通信方式*/
	void SetCommunicateType(EOutsideCommunicate::Type InType);

	void SetMaxSpeed(float InSpeed);

	/**更新通信模式，在切换模式后通常还需要执行切换通信模式后的操作**/
	virtual void UpdateCommunicateType() {};

	/**TCP/UDP通信方式，设置对应的Receiver和Sender*/
	virtual void SetToTCP_UDPMode() {};

	/**手动控制模式*/
	virtual void SetToManualControlMode() {};

	/**读取Json外部文件模式*/
	virtual void SetToJsonMode() {};

	/**读取Xml外部文件模式*/
	virtual void SetToXmlMode() {};

	//下面都是基本的移动
	virtual void MoveForward(float Val);

	virtual void MoveRight(float Val);

	//移动方法内部具体实现，可以在派生类中重写
	virtual void MoveForwardImpl(float Val);

	virtual void MoveRightImpl(float Val);

	/**获取平台信息*/
	struct FBaseActorData* GetPlatformData() { return PlatformData; };

	/**设置平台信息*/
	void SetPlatformData(FName InID, ESQBTeam::Type InTeam);

	/**宏测试*/
	/*下面的宏中间不要加任何东西，否则会影响内存定位错误，程序会崩溃*/
	/*WH_FUN_DEFINE_BEGIN();

	WH_FUN_DEFINE(FlightPlatform, true, SendPosInfo, void, FVector, PlatformPos, float, FlySpeed);

	WH_FUN_DEFINE(BaseActor, false, SendID, void, FName, ID);

	WH_FUN_DEFINE_END(2);*/

	//GET_SPECIFIED_PLATFORM_DATA(PlatformData.ID, TArray<FName>, AllOtherName, this);

	//BUILD_COMMUNICATE(a, a, FTest, Test, float, int);
	/**更新数据*/
	virtual void UpdatePlatformData();

private:
	/**物体移动，有过渡效果，使其看起来不那么突兀*/
	void MoveTick(float DeltaTime);

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ActorMesh;

	/**基础场景，为场景的根*/
	UPROPERTY()
	class USceneComponent* BaseScene;

	FVector CurRelativeLoc = FVector::ZeroVector;

	/**物体高度*/
	float Height;

	/**该物体是否在储物柜中*/
	int32 bInLocker : 1;

	uint32 bIsInMove : 1;

	/**物体的停留位置*/
	FVector OriginLocation;

	/**目标地点，注意这是相对位置*/
	FVector DestLocation;

	struct FPopMenuInfo InfoInMenu;

	/**外部通信模式*/
	EOutsideCommunicate::Type CommunicateType;

	class UPawnMovementComponent* ModuleMovement;

	class UCameraComponent* ViewCamera;

	/** 平台信息，包含平台的一些基础信息*/
	FBaseActorData* PlatformData;

protected:
		/**平台类型*/
	EPlatformCategory::Type PlatformType;

private:
	FVector RelativeLoc;

	FVector RelativeRot;

	/**控制该物体的Controller*/
	class AMainController* OwnerController;

	/**物体移动的平面，因为要根据鼠标位置移动，所以目前只使用二维平面的移动*/
	FPlane MovePlane;

	FVector Offset;

	uint32 MoveWithCursor : 1;

	/**是否从PopMenu删除*/
	uint8 bDestroyedFromPopMenu : 1;

	class TSharedPtr<class SPopMenuWidget> PopMenu;

	/**该模块所在平台，该成员可以为空*/
	class AInventoryActor * OwnerPltform;
};
