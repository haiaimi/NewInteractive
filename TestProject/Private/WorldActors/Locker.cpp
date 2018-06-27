// Fill out your copyright notice in the Description page of Project Settings.

#include "Locker.h"
#include "Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "Common/TestProjectHelper.h"
#include "Components/SpotLightComponent.h"
#include "InventoryActor.h"
#include "TestProject.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerInput.h"
#include "Engine/LocalPlayer.h"

static const FVector2D LockerShowPos(0.5f, 0.2f);     //��ʾλ��
static const FVector2D LockerHidePos(0.5f, 0.0f);      //����λ��

// Sets default values
ALocker::ALocker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LockerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LockerMesh"));
	BaseScene = CreateDefaultSubobject<USceneComponent>(TEXT("BaseScene"));
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/StarterContent/Architecture/Floor_400x400"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Game/StarterContent/Materials/M_Wood_Oak"));
	if (!ensure(MeshFinder.Succeeded()))return;	
	if (!ensure(MaterialFinder.Succeeded()))return;

	LockerMesh = MeshFinder.Object;
	LockerMaterial = MaterialFinder.Object;

	if (LockerMeshComponent && BaseScene)
	{
		LockerMeshComponent->SetCollisionResponseToChannel(COLLISION_LOCKERRAY, ECollisionResponse::ECR_Block);
		LockerMeshComponent->SetCollisionResponseToChannel(COLLISION_INVENTORYRAY, ECollisionResponse::ECR_Ignore);
		
		RootComponent = BaseScene;
		LockerMeshComponent->SetupAttachment(BaseScene);
		LockerMeshComponent->SetStaticMesh(LockerMesh);
		LockerMeshComponent->SetMaterial(0, LockerMaterial);
		FVector LockerBound = LockerMeshComponent->Bounds.BoxExtent;
		LockerMeshComponent->SetRelativeLocation(FVector(-LockerBound.X, -LockerBound.Y, 0.f));
		SetActorScale3D(FVector(0.4f, 1.5f, 1.f));
		LockerLength = LockerBound.Y * 2.f * 1.5f;
		LockerWidth = LockerBound.X * 2.f * 0.4f;

		if (SpotLight)
		{
			SpotLight->Mobility = EComponentMobility::Type::Movable;
			SpotLight->SetupAttachment(BaseScene);
			SpotLight->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
			SpotLight->IntensityUnits = ELightUnits::Candelas;
			SpotLight->Intensity = 160.f;
			SpotLight->OuterConeAngle = 24.f;
		}
	}

	LockerCapacity = 4;    //Ĭ�ϴ���������Ϊ4
	OwnerController = nullptr;
	bInMove = false;
	bInShow = false;       //Ĭ������ʾ��
	bCanUpdate = false;
	bStartTraceLine = false;
	//TestProjectHelper::Debug_ScreenMessage(FString::Printf(TEXT("Locker Length: %f, Locker Width: %f"), LockerLength, LockerWidth));

	LockerMeshComponent->OnInputTouchBegin.AddDynamic(this, &ALocker::BeginMove);
	LockerMeshComponent->OnInputTouchEnd.AddDynamic(this, &ALocker::EndMove);
	LockerMeshComponent->OnInputTouchLeave.AddDynamic(this, &ALocker::EndMove);
}

// Called when the game starts or when spawned
void ALocker::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateRelativePosToPawn(nullptr, 0);
	LockerContent.SetNum(4);      //���ô���������
	if (GetOwner())
	{
		OwnerController = Cast<AMainController>(GetOwner());
	}

	if (GEngine && GEngine->GameViewport)
	{
		FViewport* Viewport = GEngine->GameViewport->Viewport;
		Viewport->ViewportResizedEvent.AddUObject(this, &ALocker::UpdateRelativePosToPawn);
	}
}

// Called every frame
void ALocker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInMove && OwnerController && !bCanUpdate)
	{
		FVector NewRelativeLoc, DestRelPos;
		DestRelPos = bInShow ? RelativeToPawn_Show : RelativeToPawn_Hide;

		if (bInShow)       //չʾ״̬
			NewRelativeLoc = FMath::VInterpTo(GetRelativeLocationToPawn(), RelativeToPawn_Show, DeltaTime, 5.f);
		else
			NewRelativeLoc = FMath::VInterpTo(GetRelativeLocationToPawn(), RelativeToPawn_Hide, DeltaTime, 5.f);

		SetActorRelativeLocation(NewRelativeLoc);

		if ((GetRelativeLocationToPawn() - DestRelPos).Size() <= 0.01f)
		{
			SetActorRelativeLocation(DestRelPos);
			bInMove = false;
		}
	}

	UpdateMove();
}

void ALocker::AddInventoryThing(class AInventoryActor* AddedActor, FVector CursorLastPoint)
{
	FVector LockerCenter = GetActorLocation();    //��ȡ���������λ��
	const float CursorToLeftOffsetY = CursorLastPoint.Y - (LockerCenter.Y - LockerLength / 2);   //��굽���������Y�����ƫ��
	const float PerGridWidth = LockerLength / LockerCapacity;    //�����ÿ������Ŀ��
	int32 ArrayIndex = static_cast<int32>(CursorToLeftOffsetY / PerGridWidth);
	bool bCanAddToLocker = false;

	if (LockerContent[ArrayIndex])
	{
		for (int32 i = 0; i < LockerContent.Num(); ++i)
		{
			if (!LockerContent[i])
			{
				bCanAddToLocker = true;
				LockerContent[i] = AddedActor;
				ArrayIndex = i;
				break;  //����ѭ��
			}
		}
	}
	else
	{
		LockerContent[ArrayIndex] = AddedActor;
		bCanAddToLocker = true;
	}

	if (bCanAddToLocker)
	{
		AddedActor->bInLocker = true;       //���������ڴ������
		AddedActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);   //�Ѳֿ����帽���ڴ������
		FVector DestLocation(LockerCenter.X, LockerCenter.Y - LockerLength / 2 + ArrayIndex * PerGridWidth + PerGridWidth / 2, LockerCenter.Z + AddedActor->GetHeight() / 2);
		//AddedActor->SetActorLocation(DestLocation);  //���òֿ�����ľ���λ��
		AddedActor->bIsInMove = true;
		AddedActor->DestLocation = DestLocation;   //�����µ�Ŀ��λ��
	}
	else
	{
		AddedActor->bIsInMove = true;
		AddedActor->DestLocation = AddedActor->OriginLocation;
	}
}

void ALocker::RemoveInventoryThing(class AInventoryActor* RemovedActor)
{
	int32 RemoveIndex = LockerContent.Find(RemovedActor);
	LockerContent[RemoveIndex] = nullptr;
}

void ALocker::CastLight(class AInventoryActor* CastedActor)
{
	//��ȡ3����ķ���
	SpotLight->Intensity = 160.f;
	SpotLight->UpdateComponentToWorld();
	const FMatrix LockerRot = FRotationMatrix(GetActorRotation());
	const FVector DirX = LockerRot.GetUnitAxis(EAxis::Type::X);
	const FVector DirY = LockerRot.GetUnitAxis(EAxis::Type::Y);
	const FVector DirZ = LockerRot.GetUnitAxis(EAxis::Type::Z);

	int32 CastedActorIndex = LockerContent.Find(CastedActor);
	const float PerGridWidth = 150.f;
	float RelativeY = CastedActorIndex * PerGridWidth + 0.5f * PerGridWidth - (LockerLength * 0.5f);    //Y�������Ծ���

	FVector RelativeLoc = FVector(0.f, RelativeY, 200.f);
	SpotLight->SetWorldLocation(GetActorLocation() + DirY * RelativeLoc + DirZ * 200.f);
	SpotLight->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
}

void ALocker::StopCastLight()
{
	SpotLight->SetWorldLocation(FVector(1000.f, 1000.f, 1000.f)); //�ѵƵ�������
}

void ALocker::Switch()
{
	bInMove = true;
	bInShow += 1;     //�л�Locker״̬
}

void ALocker::UpdateRelativePosToPawn(class FViewport* InViewport, uint32 i)
{
	if (OwnerController)
	{
		FVector WorldPos, WorldDir, DestPos;
		FPlane LockerPlane(GetActorLocation(), FRotationMatrix(GetActorRotation()).GetUnitAxis(EAxis::Z));       //Locker����ƽ��

		TestProjectHelper::DeprojectScreenToWorld_SpecifyPoint(OwnerController, LockerShowPos, WorldPos, WorldDir);
		FVector IntersectionPos = FMath::LinePlaneIntersection(WorldPos, WorldPos + 1000.f*WorldDir, LockerPlane);    //����������ƽ��Ľ���
		RelativeToPawn_Show = IntersectionPos - OwnerController->GetFocalLocation();     //���λ��

		TestProjectHelper::DeprojectScreenToWorld_SpecifyPoint(OwnerController, LockerHidePos, WorldPos, WorldDir);
		IntersectionPos = FMath::LinePlaneIntersection(WorldPos, WorldPos + 1000.f*WorldDir, LockerPlane);
		RelativeToPawn_Hide = IntersectionPos + FRotationMatrix(GetActorRotation()).GetUnitAxis(EAxis::X)*(LockerWidth + 10.f) - OwnerController->GetFocalLocation();   //�������λ��
	}
}

void ALocker::SetVisibility(bool bVisible)
{
	LockerMeshComponent->SetVisibility(bVisible);
}

void ALocker::BeginMove(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	if (OwnerController)
	{
		FVector TouchPos = OwnerController->PlayerInput->Touches[FingerIndex];
		if (TouchPos.Z > 0)
		{
			FVector2D ScreenSize;
			bCanUpdate = true;
			this->FingerIndex = FingerIndex;
			
			float ScreenPercent = 1.f - (LockerWidth + 10.f) / (RelativeToPawn_Hide - RelativeToPawn_Show).Size();	
			OwnerController->GetLocalPlayer()->ViewportClient->GetViewportSize(ScreenSize);
			FVector RelativeTouchPoint = FMath::Lerp<FVector, float>(RelativeToPawn_Show, RelativeToPawn_Hide, ScreenPercent*((0.2f - TouchPos.Y / ScreenSize.Y) / 0.2f));            //���㴥�������λ��
			TouchOffset = RelativeTouchPoint - GetRelativeLocationToPawn();     // ���㴥���㵽Locker�����λ��

			UE_LOG(LogTemp, Log, TEXT("RelativeTouch: %s, RelativeLocPawn: %s"), *RelativeTouchPoint.ToString(), *GetRelativeLocationToPawn().ToString())
		}
	}
}

void ALocker::UpdateMove()
{
	if (OwnerController && bCanUpdate)
	{
		FVector TouchPos = OwnerController->PlayerInput->Touches[FingerIndex];
		if (TouchPos.Z > 0)
		{
			FVector2D ScreenSize;

			float ScreenPercent = 1.f - (LockerWidth + 10.f) / (RelativeToPawn_Hide - RelativeToPawn_Show).Size();
			OwnerController->GetLocalPlayer()->ViewportClient->GetViewportSize(ScreenSize);
			float MaxPercent = 0.2f + 0.2f * ((LockerWidth / 2) / (RelativeToPawn_Hide - RelativeToPawn_Show).Size()) / ScreenPercent;
			//TestProjectHelper::Debug_LogMessage(FString::Printf(TEXT("MaxPercent: %f"), MaxPercent));
			float ScreenPos_X = (0.2f - TouchPos.Y / ScreenSize.Y) / 0.2f;

			if ((TouchPos.Y / ScreenSize.Y) > MaxPercent)
			{
				SetActorRelativeLocation(RelativeToPawn_Show);
			}
			else
			{
				FVector RelativeTouchPoint = FMath::Lerp<FVector, float>(RelativeToPawn_Show, RelativeToPawn_Hide, ScreenPercent * ScreenPos_X);            //���㴥�������λ��
				SetActorRelativeLocation(RelativeTouchPoint - TouchOffset);
			}
		}
	}
}

void ALocker::EndMove(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	bCanUpdate = false;   //�����ƶ�
	TouchOffset = FVector::ZeroVector;
	if (OwnerController)
	{
		const FVector TouchPos = OwnerController->PlayerInput->Touches[FingerIndex];
		FVector2D ScreenSize;

		OwnerController->GetLocalPlayer()->ViewportClient->GetViewportSize(ScreenSize);
		float ScreenPos_X = TouchPos.Y / ScreenSize.Y;

		//���ݴ����¼�������λ�����ж�Locker�Ŀ���
		if (ScreenPos_X > 0.15f)
		{
			bInMove = true;
			bInShow = true;
		}
		else
		{
			bInMove = true;
			bInShow = false;
		}
	}
}

void ALocker::StartOpenLocker(const FVector2D& Point, float DownTime)
{
	if (OwnerController)
	{
		FVector2D ScreenSize;
		OwnerController->GetLocalPlayer()->ViewportClient->GetViewportSize(ScreenSize);

		if (Point.X / ScreenSize.X > 0.2f && Point.X / ScreenSize.X < 0.8f && Point.Y / ScreenSize.Y < 0.2f)       //��ӦLocker����������
		{
			bStartTraceLine = true;
			StartPoint = Point;
		}
	}
}

void ALocker::UpdateOpenLocker(const FVector2D& Point, float DownTime)
{

}

void ALocker::EndOpenLocker(const FVector2D& Point, float DownTime)
{
	if (bStartTraceLine)
	{
		bStartTraceLine = false;
		if ((Point.Y - StartPoint.Y) > 10.f && DownTime < 0.3f)
		{
			bInMove = true;
			bInShow = true;
		}
	}
}

FVector ALocker::GetRelativeLocationToPawn()
{
	if (OwnerController)
		return GetActorLocation() - OwnerController->GetFocalLocation();

	return FVector::ZeroVector;
}
