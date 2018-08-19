// Fill out your copyright notice in the Description page of Project Settings.

#include "Earth.h"
#include "ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"

AEarth::AEarth(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/StarterContent/Shapes/Shape_Sphere"));      //模拟地球的一个球体模型
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Game/StarterContent/Materials/M_Ground_Moss"));

	if (!ensure(MeshFinder.Succeeded()))return;
	if (!ensure(MaterialFinder.Succeeded()))return;
	
	if (ActorMesh)
	{
		ActorMesh->SetStaticMesh(MeshFinder.Object);
		ActorMesh->SetMaterial(0, MaterialFinder.Object);
	}
	
	if (ActorMesh)
	{
		FVector EarthBound = ActorMesh->Bounds.BoxExtent;
		ActorMesh->SetRelativeLocation(FVector(0.f, 0.f, -EarthBound.Z));   //地球的局部根坐标在球体下方，所以只需要设置其Z轴相对方向
		SetActorScale3D(FVector(0.8f, 0.8f, 0.8f));
		Height = EarthBound.Z * 2.f * 0.8f;
	}

	SupportTouchType.Add(ECustomTouchType::MoveRotateScalePinch_2P);
	SupportTouchType.Add(ECustomTouchType::Drag_1P);
	bCanRotate = false;
}

void AEarth::BeginPlay()
{
	Super::BeginPlay();
}

void AEarth::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCanRotate)
		SetActorRotation(GetActorRotation() + FRotator(0.f, 100.f * DeltaTime, 0.f));
}

void AEarth::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AEarth::BeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
	Super::BeginCursorOver(TouchedComponent);

	SetActorScale3D(FVector(0.85f, 0.85f, 0.85f));
	bCanRotate = true;
}

void AEarth::EndCursorOver(UPrimitiveComponent* TouchedComponent)
{
	Super::EndCursorOver(TouchedComponent);

	SetActorScale3D(FVector(0.8f, 0.8f, 0.8f));
	bCanRotate = false;
}
