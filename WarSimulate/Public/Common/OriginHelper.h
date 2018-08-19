// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/MainController.h"

/**
 * 该项目通用工具类，主要是静态成员，如屏幕输出和日志
 */

#define TypeToChar(T) #T
#define EnumsFromJson(FileName,Key,EnumType,Index,OutEnums) OriginHelper::GetEnumsFromJson(FileName,Key,TEXT(TypeToChar(EnumType)),Index,OutEnums);
#define SerializeEnumsToJson(Key,EnumType,InEnums) OriginHelper::SerializeEnums(Key,TEXT(TypeToChar(EnumType)),InEnums);

class WARSIMULATE_API OriginHelper
{
public:
	static void Init(const FString& RelativePath);

	static void Debug_ScreenMessage(FString&& InString, float ShowTime = 1.f, FColor FontColor = FColor::Blue);

	static void Debug_LogMessage(FString&& InString);

	///向Json中写入（序列化相关）Begin

	/** 向文件写入Json数据
	* @Param RelativePath 在游戏资源文件夹中的位置（相对于资源文件夹）
	* @Param FileName     配置文件名 
	* @Param JsonData     Json文件的字符串数据
	*/
	static bool WriteJsonToFile(const FString& FileName, const FString& JsonData);

	/** 从Json对象中获取数据*/
	static FString GetStringDatafromJsonObject(const TSharedPtr<FJsonObject>& JsonObj);

	/** 序列化数字数据
	* @Param Key 写入数据的键值
	* @Param InNum 写入的数据
	*/
	static void SerializeNumber(const FString& Key, const float InNum);

	/** 序列化向量类型*/
	static void SerializeVector(const FString& Key, const FVector& InVector);

	/** 序列化枚举*/
	static void SerializeEnums(const FString& Key, const FString& EnumName, const TArray<int32>& InEnums);

	/** 结束序列化，把Json对象写入到文件中*/
	static void FinishSerizlize(const FString& FileName);

	///向Json中写入（序列化相关）End

	///Json文件读取相关(反序列化) Begin

	/* 从文件加载信息，是加载到字符串里
	* @Param FileName     配置文件名
	* @Param OutInfo      读取输出的信息
	*/
	static bool LoadStringFromFile(const FString& FileName, FString& OutInfo);

	/** 准备Json读取，就是事先从文件读取文件，并构建Json对象用以读取文件,所以尽量一个一个文件的读，否则会浪费性能
	* @Param FileName Json文件名称
	*/
	static void PrepareJson(const FString& FileName);

	/** 获取Json中对应的具体类型的具体数据
	* @Param Index 读取Json中数据对象的序号
	*/
	static void GetNumberFromJson(const FString& FileName, const FString& Key, const int32 Index, float& OutNumber);

	/** 获取向量*/
	static void GetVectorFromJson(const FString& FileName, const FString& Key, const int32 Index, FVector& OutVector);

	/** 获取枚举，可以为多个*/
	static void GetEnumsFromJson(const FString& FileName, const FString& Key, const FString& EnumName, const int32 Index, TArray<int32>& OutEnums);

	/** 删除刚刚读取的Json相关文件,以便下次读取新的Json文件，就是读取下一个文件时必须执行该方法，不然会读取失败*/
	static void ResetJson();
	///Json文件读取相关（反序列化） End

	/**根据屏幕空间计算其所在的世界空间,只是鼠标位置*/
	static void DeprojectScreenToWorld_Cursor(const AMainController* PlayerContorl, FVector& OutWorldPos, FVector& OutWorldDir);

	/**根据指定屏幕上得点进行反投影
	*@ Param ScreenPos：屏幕上的位置，注意范围是 0-1 
	*/
	static void DeprojectScreenToWorld_SpecifyPoint(const AMainController* PlayerControl, FVector2D ScreenPos, FVector& OutWorldPos, FVector& OutWorldDir);

private:
	/** 文件相对路径（相对于游戏文件夹）*/
	static FString RelativePath;
	/** 当前解析的文件*/
	static FString CurParseFile;
	/** 解析的结果*/
	static TArray<TSharedPtr<class FJsonValue>>  JsonParser;
	/** 存放Json数据的数据，用于序列化*/
	static TArray<TSharedPtr<FJsonValue>> JsonDataArray;
};
