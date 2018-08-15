// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/CustomTouchInterface.h"
#include "Locker.generated.h"

/**
 *  ������࣬����ģ�ⴢ��񣬹������е����� 
 */
UCLASS()
class TESTPROJECT_API ALocker : public AActor, public ICustomTouchInterface
{
	GENERATED_BODY()
		
	/// Begin CustomTouchInterface
	virtual void GetTouchTypes_Implementation(TArray<TEnumAsByte<ECustomTouchType::Type>>& OutTypes) override;

	virtual void AddTouchTypes_Implementation(ECustomTouchType::Type InType) override;

	virtual void RemoveTouchTypes_Implementation(ECustomTouchType::Type InType)override;

	virtual void CanSuitTargetTouchType_Implementation(ECustomTouchType::Type TargetTouchType, bool& Out)override;

	virtual void InterfaceFunction(ECustomTouchType::Type InType)override {};
	/// End CustomTouchInterface

public:	
	// Sets default values for this actor's properties
	ALocker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	/** ������������ק����λ������������*/
	void AddInventoryThing(class AInventoryActor* AddedActor, FVector CursorLastPoint);

	void RemoveInventoryThing(class AInventoryActor* RemovedActor);

	/**������Ͷ�����*/
	void CastLight(class AInventoryActor* CastedActor);

	void StopCastLight();

	/**���ƴ���������*/
	void Switch();

	FVector GetRelativeLocationToPawn_Hide() { return RelativeToPawn_Hide; }

	void UpdateRelativePosToPawn(class FViewport* InViewport, uint32 i);

	/**����Locker�ɼ���*/
	void SetVisibility(bool bVisible);

	UFUNCTION()
	void BeginMove(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);

	void UpdateMove();

	UFUNCTION()
	void EndMove(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);

	void StartOpenLocker(const FVector2D& Point, float DownTime);

	void UpdateOpenLocker(const FVector2D& Point, float DownTime);

	void EndOpenLocker(const FVector2D& Point, float DownTime);

	/**��ȡ��Locker���InventoryActor*/
	TArray<class AInventoryActor*>& GetInventoryActorOutside();

private:
	FVector GetRelativeLocationToPawn();

private:
	/**������д�ŵĶ���*/
	UPROPERTY()
	TArray<class AInventoryActor*> LockerContent;
	
	///��������
	UPROPERTY()
	class UMaterialInterface* LockerMaterial;

	UPROPERTY()
	class UStaticMesh* LockerMesh;

	UPROPERTY()
	class UStaticMeshComponent* LockerMeshComponent;

	UPROPERTY()
	class USceneComponent* BaseScene;
	
	/**�۹�������������������*/
	UPROPERTY()
	class USpotLightComponent* SpotLight;

	class AMainController* OwnerController;

	uint8 bInMove : 1;

	uint8 bInShow : 1;

	uint8 bCanUpdate : 1;

	/**���켣�Ŀ�ʼ�����ڼ���Ƿ񴥷�Locker*/
	uint8 bStartTraceLine : 1;

	/**������ϵĵ�*/
	FVector2D StartPoint;

	/**Locker������ʾʱ�����Pawn��λ��*/
	FVector RelativeToPawn_Show;

	/**Locker����ʱ�����Pawn��λ��*/
	FVector RelativeToPawn_Hide;

	/**����Touch�¼���Locker�����봥����ľ���*/
	FVector TouchOffset;      

	/**��ǰ������Index��Ϊ�˴�Controller��ȡ������λ��*/
	ETouchIndex::Type FingerIndex;

public:
	float LockerLength;

	float LockerWidth;

	/***/
	int32 LockerCapacity;
};
