// Fill out your copyright notice in the Description page of Project Settings.

#include "Earth.h"
#include "ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"

AEarth::AEarth()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/StarterContent/Shapes/Shape_Sphere"));      //ģ������һ������ģ��
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
		ActorMesh->SetRelativeLocation(FVector(0.f, 0.f, -EarthBound.Z));   //����ľֲ��������������·�������ֻ��Ҫ������Z����Է���
	}
	SetActorScale3D(FVector(0.6f, 0.6f, 0.6f));

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
		SetActorRotation(GetActorRotation() + FRotator(0.f, 100.f*DeltaTime, 0.f));
}

void AEarth::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AEarth::BeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
	SetActorScale3D(FVector(1.01f, 1.01f, 1.01f));
	bCanRotate = true;
}

void AEarth::EndCursorOver(UPrimitiveComponent* TouchedComponent)
{
	SetActorScale3D(FVector(1.f, 1.f, 1.f));
	bCanRotate = false;
}