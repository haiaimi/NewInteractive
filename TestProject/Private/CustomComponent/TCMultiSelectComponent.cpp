// Fill out your copyright notice in the Description page of Project Settings.

#include "TCMultiSelectComponent.h"
#include "InventoryActor.h"
#include "TouchHelper.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/PostProcessVolume.h"
#include "EngineUtils.h"
#include "TestProjectHelper.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "SMultiSelectWidget.h"

// Sets default values for this component's properties
UTCMultiSelectComponent::UTCMultiSelectComponent():
	PostProcessVoulme(nullptr),
	bStartSelect(false)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTCMultiSelectComponent::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<APostProcessVolume> Iter(GetWorld()); Iter; ++Iter)
	{
		PostProcessVoulme = *Iter;
	}
}

void UTCMultiSelectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTCMultiSelectComponent::OnMultiSelectPressed(const FVector2D& Point, float DownTime)
{
	bStartSelect = true;
	AnchorPoint = Point;
}

void UTCMultiSelectComponent::OnMultiSelectUpdate(const FVector2D& Point, float DownTime)
{
	if (!bStartSelect)
		return;

	static EMultiSelectAnchor::Type CurAnchor = EMultiSelectAnchor::ETop_Left;
	CurPoint = Point;
	const FVector2D Sub = Point - AnchorPoint;

	if (!MultiSelectWidget.IsValid())
	{
		FMultiSelectInfo WidgetInfo;
		FVector2D ViewportSize;
		if (GEngine)
			GEngine->GameViewport->GetViewportSize(ViewportSize);

		if (Sub.X >= 0 && Sub.Y >= 0)
		{
			WidgetInfo.HorizontalAlignment = EHorizontalAlignment::HAlign_Left;
			WidgetInfo.VerticalAlignment = EVerticalAlignment::VAlign_Top;
			WidgetInfo.Padding = FMargin(AnchorPoint.X*1920.f / ViewportSize.X,
										 AnchorPoint.Y*1080.f / ViewportSize.Y,
										 0.f,
										 0.f);
		}
		else if (Sub.X > 0 && Sub.Y < 0)
		{
			WidgetInfo.HorizontalAlignment = EHorizontalAlignment::HAlign_Left;
			WidgetInfo.VerticalAlignment = EVerticalAlignment::VAlign_Bottom;
			WidgetInfo.Padding = FMargin(AnchorPoint.X*1920.f / ViewportSize.X,
										 0.f,
										 0.f,
										 1080.f - AnchorPoint.Y*1080.f / ViewportSize.Y);
		}
		else if (Sub.X < 0.f && Sub.Y > 0.f)
		{
			WidgetInfo.HorizontalAlignment = EHorizontalAlignment::HAlign_Right;
			WidgetInfo.VerticalAlignment = EVerticalAlignment::VAlign_Top;
			WidgetInfo.Padding = FMargin(0.f,
										 AnchorPoint.Y*1080.f / ViewportSize.Y,
										 1920.f - AnchorPoint.X*1920.f / ViewportSize.X,
										 0.f);
		}
		else
		{
			WidgetInfo.HorizontalAlignment = EHorizontalAlignment::HAlign_Right;
			WidgetInfo.VerticalAlignment = EVerticalAlignment::VAlign_Bottom;
			WidgetInfo.Padding = FMargin(0.f,
										 0.f,
										 1920.f - AnchorPoint.X*1920.f / ViewportSize.X,
										 1080.f - AnchorPoint.Y*1080.f / ViewportSize.Y);
		}
		
		CurAnchor = GetAnchorTypeFromSub(Sub);
		WidgetInfo.Width.BindUObject(this, &UTCMultiSelectComponent::GetSelectBoundWidth);
		WidgetInfo.Height.BindUObject(this, &UTCMultiSelectComponent::GetSelectBoundHeight);

		SAssignNew(MultiSelectWidget, SMultiSelectWidget)
		.SelectWidgetInfo(WidgetInfo);

		if (MultiSelectWidget.IsValid())
		{
				GEngine->GameViewport->AddViewportWidgetContent(
					SNew(SWeakWidget).
					PossiblyNullContent(MultiSelectWidget.ToSharedRef()), 
					0
					);
		}
	}

	if (GetAnchorTypeFromSub(Sub) != CurAnchor)
	{
		if (MultiSelectWidget.IsValid())
			MultiSelectWidget.Reset();      //删除当前多选框，重新生成
	}
}

void UTCMultiSelectComponent::OnMultiSelectReleased(const FVector2D& Point, float DownTime, TArray<AActor*>& MultiSelectedActors)
{
	if (!bStartSelect)
		return;

	MultiSelectedActors.Empty();   //清空数组
	bStartSelect = false;
	TArray<UClass*> DetectClass = { AInventoryActor::StaticClass() };
	TouchHelper::GetAllActorsInOrIntersectFrustrum(GetWorld()->GetFirstPlayerController(), AnchorPoint, Point, MultiSelectedActors, DetectClass);
	
	ShowHighlight(true);

	for (auto Iter : MultiSelectedActors)
	{
		UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent> (Iter->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		MeshComponent->SetRenderCustomDepth(true);
		TestProjectHelper::Debug_ScreenMessage(Iter->GetName());
	}

	if (MultiSelectWidget.IsValid())
		MultiSelectWidget.Reset();
}

void UTCMultiSelectComponent::ShowHighlight(bool bShow)
{
	if (PostProcessVoulme)
		PostProcessVoulme->bEnabled = bShow;
	else
		UE_LOG(LogTemp, Error, TEXT("需要在世界中放置后处理体（PostProcessVolume）并指定高亮后处理材质，否则不会出现高亮。"))
}

EMultiSelectAnchor::Type UTCMultiSelectComponent::GetAnchorTypeFromSub(const FVector2D& Sub)
{
	EMultiSelectAnchor::Type CurAnchor;
	if (Sub.X >= 0 && Sub.Y >= 0)
	{
		CurAnchor = EMultiSelectAnchor::ETop_Left;
	}
	else if (Sub.X > 0 && Sub.Y < 0)
	{
		CurAnchor = EMultiSelectAnchor::EBotton_Left;
	}
	else if (Sub.X < 0.f && Sub.Y > 0.f)
	{
		CurAnchor = EMultiSelectAnchor::ETop_Right;
	}
	else
	{
		CurAnchor = EMultiSelectAnchor::EBotton_Right;
	}

	return CurAnchor;
}

FOptionalSize UTCMultiSelectComponent::GetSelectBoundWidth()const 
{
	float Width = 0.f;
	const FVector2D Sub = CurPoint - AnchorPoint;
	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		Width = FMath::Abs(Sub.X) * 1920.f / ViewportSize.X;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("未成功获取GEngine！"))
	}

	return FOptionalSize(Width);
}

FOptionalSize UTCMultiSelectComponent::GetSelectBoundHeight()const
{
	float Height = 0.f;
	const FVector2D Sub = CurPoint - AnchorPoint;
	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		Height = FMath::Abs(Sub.Y) * 1080.f / ViewportSize.Y;
	}
	{
		UE_LOG(LogTemp, Error, TEXT("未成功获取GEngine！"))
	}

	return FOptionalSize(Height);
}