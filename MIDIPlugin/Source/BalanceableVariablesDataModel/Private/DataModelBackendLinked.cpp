#include "DataModelBackendLinked.h"

#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "DataModelLoadingHelper.h"

FDataModelBackendLinked::FDataModelBackendLinked() : FDataModel()
{
	TArray<FBalanceableVariableInfo>* AllFloats = UBalanceableVariablesController::GetAllCurrentFloatIdentifier();

	bSuccessfullyLoaded = LoadDataFromDisk();
	TArray<FNamedId> BoundIds;
	for (auto VariablesInComposition : BalanceableVariablesInComposition)
	{
		BoundIds.Append(VariablesInComposition.Value);
	}
	
	for (int i = 0; i < AllFloats->Num(); ++i)
	{
		const FBalanceableVariableInfo Info = (*AllFloats)[i];
		const FNamedId NamedId = FNamedId::NewFNamedId(Info.Name, Info.Identifier);
		if (!bSuccessfullyLoaded || !BoundIds.Contains(NamedId))
			FreeVariables.Add(NamedId);
	}	
	
}

const TDataModelFeedback<bool> FDataModelBackendLinked::AddNewBalanceableVariableTo(const FString& CompositionName,
	const FNamedId DefaultData)
{
	float Value = 0;
	TDataModelFeedback<bool> Result;
	if (UBalanceableVariablesController::TryGetCurrentValue(DefaultData.GetId(), Value))
	{
		Result = CheckAndBindVariableTo(CompositionName, DefaultData);

		auto BalancingMap = BalanceableVariableDataForBalancingInComposition.Find(CompositionName);

		float LowerValue;
		float UpperValue;
		if (Value < 0)
		{
			LowerValue = Value * 10.f;
			UpperValue = Value * 0.1f;
		}
		else if (Value > 0)
		{
			LowerValue = Value * 0.1f;
			UpperValue = Value * 10.f;;
		}
		else
		{
			LowerValue = -5.f;
			UpperValue = 5.f;
		}

		if (Result.Success() && BalancingMap != nullptr)
		{
			// Add Default Data to all balancings

			TArray<FString> Keys = TArray<FString>();
			BalancingMap->GetKeys(Keys);
			for (auto Key : Keys)
			{
				auto var = new FBalancingData();
				var->NamedId = DefaultData;
				var->LowerBound = LowerValue;
				var->CurrentValue = Value;
				var->UpperBound = UpperValue;
				BalancingMap->Find(Key)->Add(MakeShareable(var));
			}
		}
	}
	else
	{
		Result = FDataModel::AddNewBalanceableVariableTo(CompositionName, DefaultData);
	}

	return Result;
}

const TDataModelFeedback<bool> FDataModelBackendLinked::SetActiveBalancing(const FString& CompositionName,
                                                                           const FString& BalancingName)
{
	const TDataModelFeedback<bool> Result = FDataModel::SetActiveBalancing(CompositionName, BalancingName);

	if (Result.Success())
	{
		const TArray<TSharedRef<FBalancingData>>* ActiveData = BalanceableVariableDataForBalancingInComposition.
		                                                       Find(CompositionName)
		                                                       ->Find(BalancingName);
		if (ActiveData != nullptr)
		{
			for (int i = 0; i < ActiveData->Num(); ++i)
			{
				const auto Data = (*ActiveData)[i].Get();
				UBalanceableVariablesController::IssueFloatUpdate(Data.NamedId.GetId(), Data.CurrentValue);
			}
		}
	}

	return Result;
}

const TDataModelFeedback<FBalancingData> FDataModelBackendLinked::UpdateBalancingValue(const FString& CompositionName,
	const FString& BalancingName, const FNamedId Id, float NewValue)
{
	const TDataModelFeedback<FBalancingData> Result = FDataModel::UpdateBalancingValue(
		CompositionName, BalancingName, Id, NewValue);

	if (Result.Success())
	{
		const FBalancingData Data = Result.GetDataOrThrow();
		const bool bSuccess =
			UBalanceableVariablesController::IssueFloatUpdate(Data.NamedId.GetId(), Data.CurrentValue);
		if (!bSuccess) UE_LOG(LogTemp, Error, TEXT("ERROR: UBalanceableVariablesController is not inizialized!"));
	}
	SaveDataToDisk();

	return Result;
}

const TDataModelFeedback<FBalancingData> FDataModelBackendLinked::UpdateBalancingLowerBound(
	const FString& CompositionName, const FString& BalancingName, const FNamedId Id, float NewValue)
{
	const TDataModelFeedback<FBalancingData> Result = FDataModel::UpdateBalancingLowerBound(
		CompositionName, BalancingName, Id, NewValue);

	if (Result.Success())
	{
		const FBalancingData Data = Result.GetDataOrThrow();
		const bool bSuccess =
			UBalanceableVariablesController::IssueFloatUpdate(Data.NamedId.GetId(), Data.CurrentValue);
		if (!bSuccess) UE_LOG(LogTemp, Error, TEXT("ERROR: UBalanceableVariablesController is not inizialized!"));
	}

	return Result;
}

const TDataModelFeedback<FBalancingData> FDataModelBackendLinked::UpdateBalancingUpperBound(
	const FString& CompositionName, const FString& BalancingName, const FNamedId Id, float NewValue)
{
	const TDataModelFeedback<FBalancingData> Result = FDataModel::UpdateBalancingUpperBound(
		CompositionName, BalancingName, Id, NewValue);

	if (Result.Success())
	{
		const FBalancingData Data = Result.GetDataOrThrow();
		const bool bSuccess =
			UBalanceableVariablesController::IssueFloatUpdate(Data.NamedId.GetId(), Data.CurrentValue);
		if (!bSuccess) UE_LOG(LogTemp, Error, TEXT("ERROR: UBalanceableVariablesController is not inizialized!"));
	}

	return Result;
}

void FDataModelBackendLinked::HandleNewData(const FBalanceableVariableInfo Data)
{
	const FNamedId NewId = FNamedId::NewFNamedId(Data.Name, Data.Identifier);
	if (FreeVariables.Contains(NewId)) return;
	FreeVariables.Add(NewId);
}

/*** SERIALIZATION ***/
#define SEPARATOR_STRING "_|_"
#define MODULE_SAVE_PATH "../Plugins/MIDIGameMixer/BalancingHub_data.json"

void FDataModelBackendLinked::SaveDataToDisk()
{
	// Code reference: https://www.programmersought.com/article/58373450324/
	const TSharedPtr<FJsonObject> SaveData = MakeShareable(new FJsonObject());

	SaveActiveBalancingPerComposition(SaveData);
	SaveBalanceableVariableDataForBalancingInComposition(SaveData);
	SaveBalanceableVariablesInComposition(SaveData);
	
	//Save the file
	const FString FilePath = FPaths::ProjectContentDir() + TEXT(MODULE_SAVE_PATH);
	FString JSONStr;
	const TSharedRef<TJsonWriter<TCHAR>> JSONWriter = TJsonWriterFactory<TCHAR>::Create(&JSONStr);
	FJsonSerializer::Serialize(SaveData.ToSharedRef(), JSONWriter);
	FFileHelper::SaveStringToFile(JSONStr, *FilePath);
}

bool FDataModelBackendLinked::LoadDataFromDisk()
{
	const FString FilePath = FPaths::ProjectContentDir() + TEXT(MODULE_SAVE_PATH);
	if (FPaths::FileExists(FilePath))
	{
		FString FileStr;
		FFileHelper::LoadFileToString(FileStr, *FilePath);
		TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject());
		const TSharedRef<TJsonReader<>> JSONReader = TJsonReaderFactory<>::Create(FileStr);
		if (FJsonSerializer::Deserialize(JSONReader, RootObject))
		{
			TMap<FString, FString> LoadedActiveBalancingPerComposition;
			bool bSuccess = FDataModelLoadingHelper::LoadActiveBalancingPerComposition(
				RootObject, LoadedActiveBalancingPerComposition);

			TMap<FString, TMap<FString, TArray<TSharedRef<FBalancingData>>>>
				LoadedBalanceableVariableDataForBalancingInComposition;
			bSuccess &= FDataModelLoadingHelper::LoadBalanceableVariableDataForBalancingInComposition(
				RootObject, LoadedBalanceableVariableDataForBalancingInComposition);

			TMap<FString, TArray<FNamedId>> LoadedBalanceableVariablesInComposition;
			bSuccess &= FDataModelLoadingHelper::LoadBalanceableVariablesInComposition(
				RootObject, LoadedBalanceableVariablesInComposition);
			if (bSuccess)
			{
				BalanceableVariablesInComposition = LoadedBalanceableVariablesInComposition;
				ActiveBalancingPerComposition = LoadedActiveBalancingPerComposition;
				BalanceableVariableDataForBalancingInComposition =
					LoadedBalanceableVariableDataForBalancingInComposition;
			}
			return bSuccess;
		}
		return true;
	}
	return false;
}

void FDataModelBackendLinked::SaveActiveBalancingPerComposition(TSharedPtr<FJsonObject> RootObj)
{
	TArray<TSharedPtr<FJsonValue>> SerializationArray;

	for (const auto ActiveBalancing : ActiveBalancingPerComposition)
	{
		TSharedPtr<FJsonObject> ActiveBalancingObj = MakeShareable(new FJsonObject());
		ActiveBalancingObj->SetStringField(TEXT("Composition"), ActiveBalancing.Key);
		ActiveBalancingObj->SetStringField(TEXT("Balancing"), ActiveBalancing.Value);
		SerializationArray.Add(MakeShareable(new FJsonValueObject(ActiveBalancingObj)));
	}
	RootObj->SetArrayField("ActiveBalancingPerComposition", SerializationArray);
}

void FDataModelBackendLinked::SaveBalanceableVariableDataForBalancingInComposition(TSharedPtr<FJsonObject> RootObj)
{
	TArray<TSharedPtr<FJsonValue>> SerializationArray;

	for (const TTuple<FString, TMap<FString, TArray<TSharedRef<FBalancingData>>>> Composition :
	     BalanceableVariableDataForBalancingInComposition)
	{
		TSharedPtr<FJsonObject> CompositionObj = MakeShareable(new FJsonObject());
		CompositionObj->SetStringField(TEXT("CompositionName"), Composition.Key);

		TArray<TSharedPtr<FJsonValue>> CompositionDataArray;
		for (const TTuple<FString, TArray<TSharedRef<FBalancingData>>> Balancing : Composition.Value)
		{
			TSharedPtr<FJsonObject> BalancingObj = MakeShareable(new FJsonObject());
			BalancingObj->SetStringField(TEXT("BalancingName"), Balancing.Key);

			TArray<TSharedPtr<FJsonValue>> BalancingDataArray;
			for (const TSharedRef<FBalancingData> VariableData : Balancing.Value)
			{
				TSharedPtr<FJsonObject> VariableDataObj = MakeShareable(new FJsonObject());

				VariableDataObj->SetStringField(TEXT("Name"), VariableData->NamedId.GetName());
				VariableDataObj->SetStringField(TEXT("Id"), VariableData->NamedId.GetId());
				VariableDataObj->SetNumberField(TEXT("CurrentValue"), VariableData->CurrentValue);
				VariableDataObj->SetNumberField(TEXT("LowerBound"), VariableData->LowerBound);
				VariableDataObj->SetNumberField(TEXT("UpperBound"), VariableData->UpperBound);
				BalancingDataArray.Add(MakeShareable(new FJsonValueObject(VariableDataObj)));
			}
			BalancingObj->SetArrayField(TEXT("CurrentValues"), BalancingDataArray);
			CompositionDataArray.Add(MakeShareable(new FJsonValueObject(BalancingObj)));
		}
		CompositionObj->SetArrayField(TEXT("Balancings"), CompositionDataArray);
		SerializationArray.Add(MakeShareable(new FJsonValueObject(CompositionObj)));
	}
	RootObj->SetArrayField("BalanceableVariableDataForBalancingInComposition", SerializationArray);
}

void FDataModelBackendLinked::SaveBalanceableVariablesInComposition(TSharedPtr<FJsonObject> RootObj)
{
	TArray<TSharedPtr<FJsonValue>> SerializationArray;

	for (const TTuple<FString, TArray<FNamedId>> Composition : BalanceableVariablesInComposition)
	{
		TSharedPtr<FJsonObject> CompositionObj = MakeShareable(new FJsonObject());
		CompositionObj->SetStringField(TEXT("CompositionName"), Composition.Key);

		TArray<TSharedPtr<FJsonValue>> CompositionDataArray;
		for (const FNamedId VariableId : Composition.Value)
		{
			TSharedPtr<FJsonObject> VariableIdObj = MakeShareable(new FJsonObject());
			VariableIdObj->SetStringField(TEXT("Name"), VariableId.GetName());
			VariableIdObj->SetStringField(TEXT("Id"), VariableId.GetId());
			CompositionDataArray.Add(MakeShareable(new FJsonValueObject(VariableIdObj)));
		}
		CompositionObj->SetArrayField(TEXT("BoundVariables"), CompositionDataArray);
		SerializationArray.Add(MakeShareable(new FJsonValueObject(CompositionObj)));
	}
	RootObj->SetArrayField("BalanceableVariablesInComposition", SerializationArray);
}
