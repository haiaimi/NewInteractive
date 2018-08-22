// Fill out your copyright notice in the Description page of Project Settings.

#include "OriginHelper.h"
#include "Engine/Engine.h"
#include "WarSimulate/WarSimulateProject.h"
#include "Gameplay/PlatformController.h"
#include "SceneView.h"
#include "Engine/LocalPlayer.h"
#include "Paths.h"
#include "FileHelper.h"


FString OriginHelper::CurParseFile = FString(TEXT("CommonPlane.json"));    //初始化文件名称
FString OriginHelper::RelativePath = FString(TEXT("ConfigRes/FlightPlatform"));
TArray<TSharedPtr<FJsonValue>> OriginHelper::JsonParser = {};
TArray<TSharedPtr<FJsonValue>> OriginHelper::JsonDataArray = {};

void OriginHelper::Init(const FString& InRelativePath)
{
	RelativePath = InRelativePath;
}

void OriginHelper::Debug_ScreenMessage(FString&& InString, float ShowTime, FColor FontColor)
{
	GEngine->AddOnScreenDebugMessage(-1, ShowTime, FontColor, InString);
}

void OriginHelper::Debug_LogMessage(FString&& InString)
{
	UE_LOG(LogOrigin, Log, TEXT("%s"), *InString)
}

float OriginHelper::AdaptSubAngle(const float InAngle1, const float InAngle2)
{
	const float TempSub = InAngle1 - InAngle2;

	if (TempSub > 180.f)
		return 360.f - TempSub;
	else if (TempSub < -180.f)
		return 360.f + TempSub;
	else return TempSub;
}

bool OriginHelper::WriteJsonToFile(const FString& FileName, const FString& JsonData)
{
	if (JsonData.IsEmpty())return false;

	if (!FileName.IsEmpty())
	{
		FString AbsolutePath = FPaths::ProjectContentDir() + RelativePath + "/" + FileName;
		if(FFileHelper::SaveStringToFile(JsonData, *AbsolutePath))
		{
			Debug_LogMessage(TEXT("文件保存成功"));
			return true;
		}
		else
		{
			Debug_LogMessage(TEXT("文件保存失败"));
			return false;
		}
	}
	return false;
}

FString OriginHelper::GetStringDatafromJsonObject(const TSharedPtr<FJsonObject>& JsonObj)
{
	FString OutString;
	if (JsonObj.IsValid() && JsonObj->Values.Num() > 0)
	{
		TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&OutString);
		FJsonSerializer::Serialize(JsonObj.ToSharedRef(), JsonWriter);
	}
	return OutString;
}

void OriginHelper::SerializeNumber(const FString& Key, const float InNum)
{
	TSharedPtr<FJsonObject> NumObject = MakeShareable(new FJsonObject);
	NumObject->SetNumberField(Key, InNum);
	TSharedPtr<FJsonValueObject> NumValue = MakeShareable(new FJsonValueObject(NumObject));

	JsonDataArray.Add(NumValue);
}

void OriginHelper::SerializeVector(const FString& Key, const FVector& InVector)
{
	TArray<TSharedPtr<FJsonValue>> AxisArray;
	TSharedPtr<FJsonObject> VectorObject = MakeShareable(new FJsonObject);
	AxisArray.Reset();

	// 依次创建3个分量的Json数据对象
	TSharedPtr<FJsonValue> AxisValue_X = MakeShareable(new FJsonValueNumber(InVector.X));
	AxisArray.Add(AxisValue_X);
	TSharedPtr<FJsonValue> AxisValue_Y = MakeShareable(new FJsonValueNumber(InVector.Y));
	AxisArray.Add(AxisValue_Y);
	TSharedPtr<FJsonValue> AxisValue_Z = MakeShareable(new FJsonValueNumber(InVector.Z));
	AxisArray.Add(AxisValue_Z);

	VectorObject->SetArrayField(Key, AxisArray);
	//创建一个Json对象 数据类型
	TSharedPtr<FJsonValueObject> VectorDataValue = MakeShareable(new FJsonValueObject(VectorObject));  
	JsonDataArray.Add(VectorDataValue);
}

void OriginHelper::SerializeEnums(const FString& Key, const FString& EnumName, const TArray<int32>& InEnums)
{
	//存放枚举的数组
	TArray<TSharedPtr<FJsonValue>> EnumsArray;
	TSharedPtr<FJsonObject> EnumsObject = MakeShareable(new FJsonObject);

	UEnum* TargetEnum = FindObject<UEnum>(ANY_PACKAGE, *EnumName);
	if (TargetEnum)
	{
		for (auto& Iter : InEnums)
		{
			FString TempName = TargetEnum->GetNameStringByIndex(Iter);
			//Debug_LogMessage(MoveTemp(TempName));
			if (!TempName.IsEmpty())
			{
				TSharedPtr<FJsonValueString> EnumValue = MakeShareable(new FJsonValueString(TempName));
				EnumsArray.Add(EnumValue);
			}
		}
		EnumsObject->SetArrayField(Key, EnumsArray);
		TSharedPtr<FJsonValueObject> EnumsDataValue = MakeShareable(new FJsonValueObject(EnumsObject));
		JsonDataArray.Add(EnumsDataValue);
	}
}

void OriginHelper::FinishSerizlize(const FString& FileName)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetArrayField(TEXT("Origin"), JsonDataArray);

	FString JsonString = GetStringDatafromJsonObject(JsonObject);
	// 删除
	JsonString.RemoveAt(0, 13);
	JsonString.RemoveFromEnd(TEXT("}"));
	WriteJsonToFile(FileName, JsonString);
}

bool OriginHelper::LoadStringFromFile(const FString& FileName, FString& OutInfo)
{
	if (FileName.IsEmpty())return false;

	FString AbsolutePath = FPaths::ProjectContentDir() + RelativePath + "/" + FileName;
	if (FPaths::FileExists(AbsolutePath))
	{
		if (FFileHelper::LoadFileToString(OutInfo, *AbsolutePath))
			return true;      //加载成功
		else
			Debug_LogMessage(TEXT("未找到文件"));
	}

	return false;
}

void OriginHelper::PrepareJson(const FString& FileName)
{
	FString JsonContent;
	CurParseFile = FileName;
	if (LoadStringFromFile(FileName, JsonContent))
	{
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonContent);       //创建Json读取实例类

		if (FJsonSerializer::Deserialize(JsonReader, JsonParser))    //反序列化
			Debug_LogMessage(TEXT("读取成功"));
		else
			Debug_LogMessage(TEXT("读取失败"));
	}
}

void OriginHelper::GetNumberFromJson(const FString& FileName, const FString& Key, const int32 Index, float& OutNumber)
{
	//Debug_LogMessage(FString::FormatAsNumber(JsonParser.Num()));
	if (FileName.Equals(CurParseFile) && JsonParser.Num() > 0)
	{
		OutNumber = JsonParser[Index]->AsObject()->GetNumberField(Key);
	}
	else
		Debug_LogMessage(TEXT("请重新读取文件"));
}

void OriginHelper::GetVectorFromJson(const FString& FileName, const FString& Key, const int32 Index, FVector& OutVector)
{
	if (FileName.Equals(CurParseFile) && JsonParser.Num() > 0)
	{
		TArray<TSharedPtr<FJsonValue>> VectorAxisArray = JsonParser[Index]->AsObject()->GetArrayField(Key);

		if (VectorAxisArray.Num() == 3)   //向量必须要有三个分量
		{
			OutVector.X = VectorAxisArray[0]->AsNumber();
			OutVector.Y = VectorAxisArray[1]->AsNumber();
			OutVector.Z = VectorAxisArray[2]->AsNumber();
		}
		else
			Debug_LogMessage(TEXT("向量信息有误"));
	}
	else
		Debug_LogMessage(TEXT("请重新读取文件"));
}

void OriginHelper::GetEnumsFromJson(const FString& FileName, const FString& Key, const FString& EnumName, const int32 Index, TArray<int32>& OutEnums)
{
	OutEnums.Reset();
	if (FileName.Equals(CurParseFile) && JsonParser.Num() > 0)
	{
		UEnum* TargetEnum = FindObject<UEnum>(ANY_PACKAGE, *EnumName);
		TArray<TSharedPtr<FJsonValue>> EnumsArray = JsonParser[Index]->AsObject()->GetArrayField(Key);
		if (TargetEnum)
		{
			for (auto& Iter : EnumsArray)
			{
				int32 IntEnum = TargetEnum->GetValueByNameString(Iter->AsString());
				Debug_LogMessage(Iter->AsString());
				if (IntEnum != INDEX_NONE)
					OutEnums.Add(IntEnum);
			}
		}
	}
	else
		Debug_LogMessage(TEXT("请重新读取文件"));
}

void OriginHelper::ResetJson()
{
	CurParseFile.Empty();
	JsonParser.Empty();
	JsonDataArray.Empty();
}

void OriginHelper::DeprojectScreenToWorld_Cursor(const APlatformController* PlayerControl, FVector& OutWorldPos, FVector& OutWorldDir)
{
	FVector2D MousePosition;
	PlayerControl->GetMousePosition(MousePosition.X, MousePosition.Y);    //获取鼠标位置

	FSceneViewProjectionData ProjectionData;     //投影数据
	FViewport* ViewPort = PlayerControl->GetLocalPlayer()->ViewportClient->Viewport;
	PlayerControl->GetLocalPlayer()->GetProjectionData(ViewPort, eSSP_FULL, ProjectionData);      //获取当前的投影数据

	FMatrix const InvViewProjMatrix = ProjectionData.ComputeViewProjectionMatrix().InverseFast();     //获取投影矩阵的逆，这样就可以根据NDC空间得出在世界坐标系中的位置
	FSceneView::DeprojectScreenToWorld(MousePosition, ProjectionData.GetConstrainedViewRect(), InvViewProjMatrix, OutWorldPos, OutWorldDir);   //其流程就是: 屏幕坐标-->NDC坐标-->世界坐标
}

void OriginHelper::DeprojectScreenToWorld_SpecifyPoint(const APlatformController* PlayerControl, FVector2D ScreenPos, FVector& OutWorldPos, FVector& OutWorldDir)
{
	FVector2D ScreenSize;
	PlayerControl->GetLocalPlayer()->ViewportClient->GetViewportSize(ScreenSize);   //获取屏幕尺寸

	FSceneViewProjectionData ProjectionData;     //投影数据
	FViewport* ViewPort = PlayerControl->GetLocalPlayer()->ViewportClient->Viewport;
	PlayerControl->GetLocalPlayer()->GetProjectionData(ViewPort, eSSP_FULL, ProjectionData);      //获取当前的投影数据

	FMatrix const InvViewProjMatrix = ProjectionData.ComputeViewProjectionMatrix().InverseFast();     //获取投影矩阵的逆，这样就可以根据NDC空间得出在世界坐标系中的位置
	FSceneView::DeprojectScreenToWorld(ScreenSize * ScreenPos, ProjectionData.GetConstrainedViewRect(), InvViewProjMatrix, OutWorldPos, OutWorldDir);
}
