// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "OriginHelper.h"
#include "WarSimulateType.generated.h"

/**
* 该项目中常用的宏，类型，枚举，结构等等
*/

//信息发布多播代理
DECLARE_MULTICAST_DELEGATE_OneParam(FMessagePublish, FString&);

#define EXPAND(x) x
#define WH_CONCAT_(l,r) l##r
#define WH_CONCAT(l,r) WH_CONCAT_(l,r) 
#define SEPATATOR_1 ,
#define SEPATATOR_2 ;

#define DECLARENUM() int32 a = 1 SEPATATOR_1 b = 2 SEPATATOR_2

/**
* 下面的宏是用来计算宏可变参数列表的参数数量，这里最大计算6个，如果有需求可以继续添加
* 这里 _VA_ARGS__前面一定要加##，否则参数为空时会报错
*/
#define WH_INTERNAL_ARG_COUNT( \
	_0, _1, _2, _3, _4, _5, _6, N, ...) N

#define WH_ARG_COUNT(...) EXPAND(WH_INTERNAL_ARG_COUNT(0, ##__VA_ARGS__, \
	6, 5, 4, 3, 2, 1, 0))

/*计算可变参数列表的一半数目*/
#define WH_ARG_COUNT_HALF(...) EXPAND(WH_INTERNAL_ARG_COUNT(0, ##__VA_ARGS__,\
	3, 2.5, 2, 1.5, 1, 0.5, 0))


/**获取多参数列表中偶数位/奇数位的参数*/
#define WH_ARG0()
#define WH_ARG2(f,t,v) f(t,v)
#define WH_ARG4(f,t,v,...) WH_ARG2(f,t,v) SEPATATOR_1 EXPAND(WH_ARG2(f,##__VA_ARGS__))
#define WH_ARG6(f,t,v,...) WH_ARG2(f,t,v) SEPATATOR_1 EXPAND(WH_ARG4(f,##__VA_ARGS__))
#define WH_ARG8(f,t,v,...) WH_ARG2(f,t,v) SEPATATOR_1 EXPAND(WH_ARG6(f,##__VA_ARGS__))

/**遍历赋值*/
#define WH_ARG_DATA0()
#define WH_ARG_DATA2(f,t,v) f(t,v)
#define WH_ARG_DATA4(f,t,v,...) WH_ARG_DATA2(f,t,v) SEPATATOR_2 EXPAND(WH_ARG_DATA2(f,##__VA_ARGS__))
#define WH_ARG_DATA6(f,t,v,...) WH_ARG_DATA2(f,t,v) SEPATATOR_2 EXPAND(WH_ARG_DATA4(f,##__VA_ARGS__))
#define WH_ARG_DATA8(f,t,v,...) WH_ARG_DATA2(f,t,v) SEPATATOR_2 EXPAND(WH_ARG_DATA6(f,##__VA_ARGS__))

#define WH_ODD_FUN(t,v) t
#define WH_EVEN_FUN(t,v) v
#define WH_EVEN_CUSTOM_FUN_(t,v) InData->##v
#define WH_EVEN_CUSTOM_FUN(t,v) WH_EVEN_CUSTOM_FUN_(t,v)
#define WH_EVEN_CUSTOM_DATA_FUN(t,v)\
TempData->v=v

#define WH_ODD_EVEN_ARG_(v,f,...) \
		EXPAND(WH_CONCAT(v, WH_ARG_COUNT(__VA_ARGS__))(f,##__VA_ARGS__))
#define WH_ODD_EVEN_ARG(v,f,...) \
		WH_ODD_EVEN_ARG_(v,f,##__VA_ARGS__)

/**遍历多参数列表*/
#define WH_DOARG0(o)
#define WH_DOARG1(v_1,v_2,...) v_1  v_2
#define WH_DOARG2(v_1,v_2,...) WH_DOARG1(v_1,v_2) SEPATATOR_1 EXPAND(WH_DOARG1(##__VA_ARGS__))
#define WH_DOARG3(v_1,v_2,...) WH_DOARG1(v_1,v_2) SEPATATOR_1 EXPAND(WH_DOARG2(##__VA_ARGS__))
#define WH_DOARG4(v_1,v_2,...) WH_DOARG1(v_1,v_2) SEPATATOR_1 EXPAND(WH_DOARG3(##__VA_ARGS__))

#define WH_FOREACH_(...) \
        EXPAND(WH_CONCAT(WH_DOARG,WH_ARG_COUNT_HALF(__VA_ARGS__))(##__VA_ARGS__))
#define WH_FOREACH(...) \
        EXPAND(WH_FOREACH_(##__VA_ARGS__))

#define WH_DOARG_STRUCT0(o)
#define WH_DOARG_STRUCT1(v_1,v_2,...) v_1  v_2
#define WH_DOARG_STRUCT2(v_1,v_2,...) WH_DOARG_STRUCT1(v_1,v_2) SEPATATOR_2 EXPAND(WH_DOARG_STRUCT1(##__VA_ARGS__))
#define WH_DOARG_STRUCT3(v_1,v_2,...) WH_DOARG_STRUCT1(v_1,v_2) SEPATATOR_2 EXPAND(WH_DOARG_STRUCT2(##__VA_ARGS__))
#define WH_DOARG_STRUCT4(v_1,v_2,...) WH_DOARG_STRUCT1(v_1,v_2) SEPATATOR_2 EXPAND(WH_DOARG_STRUCT3(##__VA_ARGS__))

#define WH_FOREACH_STRUCT_(...) \
        EXPAND(WH_CONCAT(WH_DOARG_STRUCT,WH_ARG_COUNT_HALF(__VA_ARGS__))(##__VA_ARGS__))
#define WH_FOREACH_STRUCT(...) \
        WH_FOREACH_STRUCT_(##__VA_ARGS__)

#define FUN_ARGS(p,n) p##n

#define WH_STRUCT(FunName) F##FunName
#define WH_DELEGATE(FunName) F##FunName##Delegate

#define WH_FUN_DEFINE_BEGIN()\
TArray<FString> FunNames;\
int64 DefineStart=1;

#define WH_FUN_DEFINE_END(FunCount)\
int64 DefineEnd=1;\
const int32 CustomFunCounts=FunCount;

/**识别方法名以便后面的使用*/
#define WH_FUN_DEFINE_IMPLEMENT()\
if (GlobalBindFunctions.Num() == 0 && GlobalRemoveDelegates.Num()==0)\
{\
	GlobalBindFunctions.Reset();\
	GlobalRemoveDelegates.Reset();\
	int64* StartPoint = &this->DefineStart;\
	FString* StrPoint = nullptr;\
	BindFunctionPtr* FunPoint = nullptr;\
	RemoveDelegatePtr* RemoveDelegatePoint = nullptr;\
	StartPoint++;\
	const uint32 FunDefineWidth=((uint32)(&DefineEnd - &DefineStart)-1)/CustomFunCounts;\
	for (int32 i = 0; i < CustomFunCounts; ++i)\
	{\
		StrPoint = (FString*)(StartPoint);\
		StartPoint += 2;\
		FunPoint = (BindFunctionPtr*)(StartPoint);\
		StartPoint += 2;\
		RemoveDelegatePoint = (RemoveDelegatePtr*)(StartPoint);\
		StartPoint += FunDefineWidth-4;\
		GlobalBindFunctions.Add(FName(**StrPoint), *FunPoint);\
		GlobalRemoveDelegates.Add(FName(**StrPoint), *RemoveDelegatePoint);\
		\
	}\
}

#define WH_CUSTOM_FUN_FINISH(ClassName,FunName)\
typedef ClassName::F##FunName F##FunName;\
typedef ClassName::F##FunName##Delegate F##FunName##Delegate;

#define WH_FUN_DEFINE(PlatformType,Controlled,FunName,RetType,...)\
FString FunName##_STR=#FunName;\
BindFunctionPtr FunName##BindPtr=&AInventoryActor::Bind##FunName;\
RemoveDelegatePtr FunName##RemoveDelegatePtr=&AInventoryActor::Remove##FunName##Delegate;\
FDelegateHandle FunName##DelegateHandle;\
FUNC_DECLARE_MULTICAST_DELEGATE(F##FunName##Delegate,RetType,EXPAND(WH_ODD_EVEN_ARG(WH_ARG,WH_ODD_FUN, ##__VA_ARGS__))); \
F##FunName##Delegate FunName##Delegate; \
\
struct WH_CONCAT(F,FunName) \
{ \
	EXPAND(WH_FOREACH_STRUCT(##__VA_ARGS__)) \
	;\
	RetType* ReturnVal; \
};	\
\
UFUNCTION()\
RetType FunName(\
EXPAND(WH_FOREACH(##__VA_ARGS__))\
)\
{\
	F##PlatformType##Data* TempData = static_cast<F##PlatformType##Data*>(PlatformData);\
	if(TempData)\
	{\
		TempData->bControlled=Controlled;\
		WH_ODD_EVEN_ARG(WH_ARG_DATA, WH_EVEN_CUSTOM_DATA_FUN, ##__VA_ARGS__);\
		UpdatePlatformData();\
	}\
}\
\
UFUNCTION()\
void FunName##_Publish(F##FunName* InArg)\
{ \
	if(InArg!=nullptr)\
	{ \
		F##FunName* InData = static_cast<F##FunName*>(InArg);\
		if(FunName##Delegate.IsBound() && InData)\
		{\
			FunName##Delegate.Broadcast(EXPAND(WH_ODD_EVEN_ARG(WH_ARG,WH_EVEN_CUSTOM_FUN, ##__VA_ARGS__)));\
		}\
	}\
}\
\
UFUNCTION()\
FDelegateHandle Bind##FunName(AInventoryActor* BindActor)\
{\
	OriginHelper::Debug_ScreenMessage(TEXT("Bind Actor"),5);\
	BindActor->FunName##DelegateHandle=FunName##Delegate.AddUObject(BindActor,&AInventoryActor::##FunName);\
	return BindActor->FunName##DelegateHandle; \
}\
void Remove##FunName##Delegate(AInventoryActor* Receiver)\
{\
	FunName##Delegate.Remove(Receiver->FunName##DelegateHandle);\
}

/**具体通信的实现*/
#define WH_COMMUNICATE_IMPLEMENT(CommunicateName)\
void CommunicateName##_BuildCommunication(AInventoryActor* Sender,AInventoryActor* Receiver,FString FunName = #CommunicateName)\
{\
	if(CommunicationManager)\
	{\
		CommunicationManager->BuildCommunication(Sender,Receiver,FunName);\
		OriginHelper::Debug_ScreenMessage(Sender->GetName(), 10.f); \
		OriginHelper::Debug_ScreenMessage(Receiver->GetName(), 10.f); \
	}\
};\
void CommunicateName##_BreakCommunication(AInventoryActor* Sender,AInventoryActor* Receiver,FString FunName = #CommunicateName)\
{\
	if(CommunicationManager)\
		CommunicationManager->BreakCommunication(Sender, Receiver, FunName);\
};\
void CommunicateName##_Implementation(AInventoryActor* Sender, F##CommunicateName& InParams);

/**多参数列表测试代码*/
#define WH_TEST1__(v1,v2,...) \
int32 j = WH_CONCAT(v1,v2);

#define WH_TEST1_(v1,v2,...) \
int32 i = WH_CONCAT(v1,v2); \
EXPAND(WH_TEST1__(##__VA_ARGS__))

#define WH_TEST1(...) EXPAND(WH_TEST1_(##__VA_ARGS__))

//#define WH_ARGNAME_(arg1)\
//PlatformData.Speed.arg1##_Speed=arg1;
//#define WH_ARGNAME(arg1)WH_ARGNAME_(arg1)

#define GET_SPECIFIED_PLATFORM_DATA(PlatformID, DataType, DataName, ActorRef) \
DataType GetData_##DataName()     \
{     \
	DataType Temp;   \
	if(GetWorld())  \
	{  \
		if (USQBGameInstance* GameInstance = Cast<USQBGameInstance>(GetWorld()->GetGameInstance()))  \
		{            \
			Temp = GameInstance->GetData_##DataName(PlatformID, ActorRef); \
		}           \
	}    \
	return Temp;     \
\
}

//#define BUILD_COMMUNICATE(PublishActor, SubscribeActor, DelegateName, DelegateInstance, ...)  \
//FUNC_DECLARE_DELEGATE(DelegateName, ...)  \
//DelegateName DelegateInstance;   \
//template<typename Type1> \
//void Communicate_##DelegateName(Type1 Val1)    \
//{ \
//   \ 
//}


/**基础组件的通信模式*/
UENUM(BlueprintType)
namespace EOutsideCommunicate
{
	enum Type
	{
		EUDP_TCP,      //通过TCP/UDP来进行通信
		EManualControl,      //手动模式
		ELoadConfigFile_Json,   //通过读取配置文件
		ELoadConfigFile_Xml,
	};
}

/**各个不同平台的枚举*/
namespace EPlatformCategory
{
	enum Type
	{
		EBaseModule,      //基础模块
		EPlane,          //飞行平台
		ETank,            //陆地坦克
	};
}

/**平台各个模块枚举*/
UENUM(BlueprintType)
namespace EPlatformModule
{
	enum Type
	{
		EMissle_Air2Air,     //空对空到导弹
		EMissle_Air2Surface,  //空对地导弹
		ERadar,      //雷达
	};
}

/**本方与敌方*/
UENUM(BlueprintType)
namespace ESQBTeam
{
	enum Type
	{
		UnKnown,
		EPlayer,   //玩家本方
		EEnemy,     //敌方
	};
}

UENUM(BlueprintType)
namespace EVisualControlInputs
{
	enum Type
	{
		Roll,
		Pitch,
		Heading,
		SkidPercent,
		FlightPathUp,
		FlightPathRight,
		FlightAltitude,   //飞行海拔
		FlightSpeed,   //飞行速度
	};
}

UENUM()
namespace EFunctionName
{
	enum Type
	{
		WH_FUN,
		WH_FUN_1,
	};
}

/**平台的信息*/
USTRUCT(BlueprintType)
struct FInventoryActorData
{
	GENERATED_USTRUCT_BODY()

		/**平台ID，每个平台实例都有独立的ID号*/
		FName ID;

	/**所属队伍*/
	ESQBTeam::Type OwnerTeam;

	/**平台类型*/
	EPlatformCategory::Type PlatformType;

	/**平台所在位置*/
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="FlightData")
	FVector PlatformPos;

	bool bControlled;

	//union PlatformData_Speed
	//{
	//	float Plane_Speed;
	//	float Tank_Speed;
	//	float Ship_Speed;
	//};

	//PlatformData_Speed Speed;

	FInventoryActorData()
	{
		ID = TEXT("None");
		OwnerTeam = ESQBTeam::UnKnown;
		PlatformType = EPlatformCategory::EBaseModule;
		PlatformPos = FVector::ZeroVector;
		bControlled = false;
	}

	/**拷贝构造函数*/
	FInventoryActorData(const FInventoryActorData& InData)
	{
		*this = InData;
	}
};

/**具体的飞行平台*/
USTRUCT(BlueprintType)
struct FFlightPlatformData :public FInventoryActorData
{
	GENERATED_USTRUCT_BODY();

	/**飞行速度*/
	float FlySpeed;

	/**加速度*/
	float FlyAcceleration;

	FFlightPlatformData():
		FInventoryActorData()
	{
		FlySpeed = 0.f;
		FlyAcceleration = 0.f;
	}

	FFlightPlatformData(const FInventoryActorData& InData):
		FInventoryActorData(InData)
	{}
};