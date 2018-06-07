// Fill out your copyright notice in the Description page of Project Settings.

#include "Locker.h"
#include "Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "Common/TestProjectHelper.h"
#include "Components/SpotLightComponent.h"


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
		RootComponent = BaseScene;
		LockerMeshComponent->SetupAttachment(BaseScene);
		LockerMeshComponent->SetStaticMesh(LockerMesh);
		LockerMeshComponent->SetMaterial(0, LockerMaterial);
		FVector LockerBound = LockerMeshComponent->Bounds.BoxExtent;
		LockerMeshComponent->SetRelativeLocation(FVector(-LockerBound.X, -LockerBound.Y, 0.f));
		SetActorScale3D(FVector(0.4f, 1.5f, 1.f));
	}
}

// Called when the game starts or when spawned
void ALocker::BeginPlay()
{
	Super::BeginPlay();
	
	TestProjectHelper::Debug_ScreenMessage(RootComponent->GetName());
}

// Called every frame
void ALocker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

