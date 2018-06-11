// Fill out your copyright notice in the Description page of Project Settings.

#include "Locker.h"
#include "Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "Common/TestProjectHelper.h"
#include "Components/SpotLightComponent.h"
#include "InventoryActor.h"
#include "TestProject.h"


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
	//TestProjectHelper::Debug_ScreenMessage(FString::Printf(TEXT("Locker Length: %f, Locker Width: %f"), LockerLength, LockerWidth));
}

// Called when the game starts or when spawned
void ALocker::BeginPlay()
{
	Super::BeginPlay();
	
	LockerContent.SetNum(4);      //���ô���������
}

// Called every frame
void ALocker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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