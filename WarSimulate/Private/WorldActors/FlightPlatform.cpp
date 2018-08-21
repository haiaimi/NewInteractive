// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldActors/FlightPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerInput.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "WarSimulateProject.h"


AFlightPlatform::AFlightPlatform(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer.SetDefaultSubobjectClass<UFloatingPawnMovement>(TEXT("ModuleMovement"))),
	bInAir(false),
	TakeOffAngle(0.f),
	CurOffsetAngle_Right(0.f),
	CurOffsetAngle_Up(0.f)
{
	PlaneBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PlaneCollision"));
	PlaneBox->SetupAttachment(BaseScene);
	PlaneBox->SetBoxExtent(FVector(100.f, 46.f, 22.f));
	PlaneBox->SetCollisionResponseToAllChannels(ECR_Block);
	PlaneBox->SetCollisionResponseToChannel(COLLISION_LOCKERRAY, ECollisionResponse::ECR_Ignore);
	PlaneBox->SetCollisionResponseToChannel(COLLISION_INVENTORYRAY, ECollisionResponse::ECR_Ignore);
	RootComponent = PlaneBox;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Plane/CGModel/F18C/F-18C_Hornet"));
	if (!ensure(MeshFinder.Succeeded()))return;
	ActorMesh->SetStaticMesh(MeshFinder.Object);

	ActorMesh->SetupAttachment(PlaneBox);
	ActorMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	ActorMesh->SetRelativeLocation(FVector(0.f, 0.f, -20.f));

	ModuleMovement->SetUpdatedComponent(PlaneBox);
	ViewCamera->SetupAttachment(ActorMesh, TEXT("CameraSocket"));
	PlatformType = EPlatformCategory::EPlane;

	delete PlatformData;
	PlatformData = new FFlightPlatformData();
	PlatformData->ID = TEXT("Plane");
	ExpandPlatformData = static_cast<FFlightPlatformData*>(PlatformData);

	SupportTouchType.Add(ECustomTouchType::Drag_1P);
}

void AFlightPlatform::BeginPlay()
{
	Super::BeginPlay();

	////测试序列化
	//TArray<int32>  EnumsResult = { 0,1,2,3 };
	//FVector Pos(10.1f, 10.1f, 10.1f);
	////序列化的顺序就是根据下面的顺序
	//OriginHelper::SerializeNumber(TEXT("TEST"), 10.f);
	//OriginHelper::SerializeVector(TEXT("Position"), Pos);
	//SerializeEnumsToJson(TEXT("PlaneModlues"), EPlatformModule, EnumsResult);
	//OriginHelper::FinishSerizlize(TEXT("Test.json"));
	//OriginHelper::ResetJson();
}

void AFlightPlatform::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//设置飞行平台额外交互方式
	//飞机向上和向下
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("FlyPlatformUp", EKeys::Up, 1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("FlyPlatformUp", EKeys::Down, -1.f));

	PlayerInputComponent->BindAxis(TEXT("FlyPlatformUp"), this, &AFlightPlatform::MoveUp);
}

void AFlightPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ModuleMovement->MoveUpdatedComponent(GetActorRotation().Vector()*FlySpeed*DeltaTime/100.f, GetActorRotation().Quaternion(), true);
	//SetMaxSpeed(FlySpeed*DeltaTime);

	OriginHelper::Debug_ScreenMessage(FString::FormatAsNumber(FlySpeed));
	TakeOff(DeltaTime);
	UpdatePlatformData();

	if (Controller == NULL)MoveRightImpl(0);
	if (FlySpeed < 10000.f || !bInAir)return;
	// 飞机飞行时的晃动，提高真实性
	static float ShakeTime = 0.f;
	static float AddDir = 1.f;
	if (ShakeTime > PI)
	{
		ShakeTime = PI;
		AddDir = -1.f;
	}
	if (ShakeTime < -PI)
	{
		ShakeTime = -PI;
		AddDir = 1.f;
	}
	const float CurRelHeight = FMath::Sin(ShakeTime + PI)*AddDir;
	ViewCamera->SetRelativeLocation(FVector(0.f, ShakeTime * 0.7f, CurRelHeight)*2.f);

	ShakeTime += DeltaTime * AddDir;
}

void AFlightPlatform::BeginDestroy()
{
	Super::BeginDestroy();
}

void AFlightPlatform::UpdateCommunicateType()
{
	//根据不同的通信模式，选择不同的模拟方式
	switch (CommunicateType)
	{
		//TCP/UDP
	case EOutsideCommunicate::EUDP_TCP:
		break;

		//手动控制
	case EOutsideCommunicate::EManualControl:
		break;

		//读取Json文件
	case EOutsideCommunicate::ELoadConfigFile_Json:
		break;

		//读取xml文件
	case EOutsideCommunicate::ELoadConfigFile_Xml:
		break;
	}
}

void AFlightPlatform::SetToTCP_UDPMode()
{

}

void AFlightPlatform::SetToManualControlMode()
{

}

void AFlightPlatform::SetToJsonMode()
{
	//从飞行平台的配置文件中读取数据
	TArray<int32>  EnumsResult;
	OriginHelper::PrepareJson(FlightConfigName);
	OriginHelper::GetNumberFromJson(FlightConfigName, TEXT("FlySpeed"), 0, FlySpeed);
	OriginHelper::GetNumberFromJson(FlightConfigName, TEXT("FlyAccleration"), 1, FlyAcceleration);
	OriginHelper::GetVectorFromJson(FlightConfigName, TEXT("FlyLocation"), 2, FlyLocation);
	EnumsFromJson(FlightConfigName, TEXT("FlightModules"), EPlatformModule, 3, EnumsResult);
	OriginHelper::ResetJson();   //清空单例中的数据以便下次数据读取写入
}

void AFlightPlatform::SetToXmlMode()
{

}

void AFlightPlatform::MoveForwardImpl(float Val)
{
	FlySpeed += 1000 * Val;
}

void AFlightPlatform::MoveRightImpl(float Val)
{
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	if (Val != 0.f)
	{
		FRotator PlaneRot = GetActorRotation();
		const FQuat AddedAngle_Yaw(FVector(0.f, 0.f, 1.f), Val * FMath::DegreesToRadians(DeltaTime*20.f));
		AddActorWorldRotation(AddedAngle_Yaw);

		if (FlySpeed < 50000.f)return;
		if (CurOffsetAngle_Right >= -30.f && CurOffsetAngle_Right <= 30.f)
		{
			float DeltaAngle = Val * DeltaTime * 30.f * 1.5f;
			if (CurOffsetAngle_Right + DeltaAngle > 30.f)
				DeltaAngle = 30 - CurOffsetAngle_Right;
			else if (CurOffsetAngle_Right + DeltaAngle < -30.f)
				DeltaAngle = -30.f - CurOffsetAngle_Right;
			PlaneRot = GetActorRotation();
			const FQuat AddedAngle_Roll(FRotationMatrix(PlaneRot).GetUnitAxis(EAxis::X), -FMath::DegreesToRadians(DeltaAngle));
			AddActorWorldRotation(AddedAngle_Roll);
			CurOffsetAngle_Right += DeltaAngle;
		}
	}
	else
	{
		if (CurOffsetAngle_Right != 0.f)
		{
			CurOffsetAngle_Right = GetToCenterSubAngle();
			FRotator PlaneRot = GetActorRotation();
			float DeltaAngle = DeltaTime * (CurOffsetAngle_Right > 0.f ? -30.f : 30.f) * 1.5f;
			const float Tmp = CurOffsetAngle_Right + DeltaAngle;
			if (CurOffsetAngle_Right*Tmp <= 0.f)
			{
				DeltaAngle = 0.f - CurOffsetAngle_Right;
				CurOffsetAngle_Right = 0.f;
			}
			else
				CurOffsetAngle_Right = Tmp;
			const FQuat AddedAngle_Yaw(FRotationMatrix(PlaneRot).GetUnitAxis(EAxis::X), -FMath::DegreesToRadians(DeltaAngle));
			AddActorWorldRotation(AddedAngle_Yaw);
		}
	}
}

void AFlightPlatform::MoveUp(float Val)
{
	//
	if (FlySpeed < 40000.f)return;
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	if (Val > 0.f)bInAir = true;

	//if (Val != 0 && CurOffsetAngle_Up <= 40.f && CurOffsetAngle_Up >= -40.f)
	//{
	//	float DeltaAngle = Val * DeltaTime * 20.f;
	//	
	//	if (CurOffsetAngle_Up + DeltaAngle > 40.f)
	//	{
	//		DeltaAngle = 40.f - CurOffsetAngle_Up;
	//	}
	//	else if (CurOffsetAngle_Up + DeltaAngle < -40.f)
	//	{
	//		DeltaAngle = -40.f - CurOffsetAngle_Up;
	//	}
	//	//OriginHelper::Debug_ScreenMessage(FString::SanitizeFloat(DeltaAngle));
	//
	//}
	float DeltaAngle = Val * DeltaTime * 30.f;
	const FQuat AddedAngle_Pitch(FRotationMatrix(GetActorRotation()).GetUnitAxis(EAxis::Y), -FMath::DegreesToRadians(DeltaAngle));
	CurOffsetAngle_Up += DeltaAngle;
	AddActorWorldRotation(AddedAngle_Pitch);
}

void AFlightPlatform::UpdatePlatformData()
{
	Super::UpdatePlatformData();

	if (ExpandPlatformData->bControlled)      //当前飞机受控
	{
		if (TakeOffAngle == 0.f && !bInAir)  //飞机未起飞则先起飞
			TakeOffAngle = 0.001f;
		if (!bInAir)return;   //等待飞机升空
		else TakeOffAngle = 0.f;

		FlySpeed = 250000.f;
		const FVector TargetVec = PlatformData->PlatformPos - GetActorLocation();
		const FVector PlainForwardVec = GetActorRotation().Vector();
		const FVector PlainRightVec = FRotationMatrix(GetActorRotation()).GetUnitAxis(EAxis::Y);

		const FVector TargetVec2D = TargetVec.GetSafeNormal2D();
		const FVector PlainForwardVec2D = PlainForwardVec.GetSafeNormal2D();
		const FVector PlainRightVec2D = PlainRightVec.GetSafeNormal2D();

		const float SubDegree_Hori = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(TargetVec2D, PlainForwardVec2D)));
		const float TargetZDegree = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FVector(0.f, 0.f, 1.f), TargetVec.GetSafeNormal())));
		const float PlainZDegree = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FVector(0.f, 0.f, 1.f), PlainForwardVec)));

		//判断在左边还是右边，从而判断飞机的转向
		if (FMath::Abs(SubDegree_Hori) > 2.f)
		{
			float Temp = FVector::DotProduct(TargetVec2D, PlainRightVec2D);
			if (Temp >= 0)
			{
				MoveRightImpl(1.f);
				MoveRightImpl(1.f);
			}
			else if (Temp < 0)
			{
				MoveRightImpl(-1.f);
				MoveRightImpl(-1.f);
			}
		}

		if (FMath::Abs(TargetZDegree - PlainZDegree) > 1.f)
			if (PlainZDegree < TargetZDegree)
				MoveUp(-1.f);
			else
				MoveUp(1.f);
	}
}

int32 AFlightPlatform::EventTest(float Speed, int32 Num)
{
	return Num;
}

FVector AFlightPlatform::GetUpVector()
{ 
	FVector Up = FRotationMatrix(GetActorRotation()).GetUnitAxis(EAxis::Z);
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + Up * 200, FColor::Green, false, 20.f, 0, 2.f);
	const FQuat Temp = FQuat(FRotationMatrix(GetActorRotation()).GetUnitAxis(EAxis::X), FMath::DegreesToRadians(CurOffsetAngle_Right*1.5f));
	Up = Temp.RotateVector(Up);

	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + Up * 200, FColor::Black, false, 5.f, 0, 2.f);
	return Up;
}

float AFlightPlatform::GetToCenterSubAngle()
{
	const FMatrix PlainRotationMat = FRotationMatrix(GetActorRotation());
	const FVector PlainUp = PlainRotationMat.GetUnitAxis(EAxis::Z);
	//const FVector PlainRight = PlainRotationMat.GetUnitAxis(EAxis::Y);
	const FVector PlainForward = PlainRotationMat.GetUnitAxis(EAxis::X);

	const FVector PlaneNormal = FVector::CrossProduct(PlainForward, FVector(0.f, 0.f, 1.f));
	const FPlane CenterPlane(FVector::ZeroVector, PlaneNormal);

	float SubAngle = FMath::RadiansToDegrees(FMath::Acos(CenterPlane.PlaneDot(PlainUp)));
	return SubAngle - 90.f;
}

void AFlightPlatform::TakeOff(float DeltaTime)
{
	if (TakeOffAngle == 0.f)return;
	if (TakeOffAngle > 0.f)  //下面是转弯
	{
		//下面的方法不太行
		/*const float NextAngle = FMath::FInterpConstantTo(TakeOffAngle, 90.f, DeltaTime, 8.f);
		const FVector CurVec = FRotator(0.f, 180.f - TakeOffAngle, 0.f).Vector()*570.f;
		const FVector NextVec = FRotator(0.f, 180.f - NextAngle, 0.f).Vector()*570.f;
		FVector MoveVec = NextVec - CurVec;
		MoveVec.Y += 30.f * DeltaTime;

		SetActorRotation(FRotator(0.f, NextAngle, 0.f));
		SetActorLocation(GetActorLocation() + MoveVec);
		TakeOffAngle = NextAngle;*/

		//使用这个简单粗暴的方法
		FlySpeed = 20000.f;
		MoveRightImpl(1.f);
		if (FMath::Abs(GetActorRotation().Yaw - 90.f) < 1.f)
			TakeOffAngle = -1.f;
	}
	else if (TakeOffAngle < 0.f)   //下面的就是加速起步
	{
		FlySpeed += 100000.f*DeltaTime;

		if (FlySpeed > 100000.f)
		{
			MoveUp(1.f);
			if (FlySpeed > 150000.f)
			{
				TakeOffAngle = 0.f;   //起飞完成
				bInAir = true;
			}
		}
	}
}
