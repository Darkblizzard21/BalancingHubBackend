// Fill out your copyright notice in the Description page of Project Settings.


#include "DataModel.h"
#include "DataModelErrorMacros.h"

FDataModel::FDataModel()
{
	FreeVariables = TArray<FNamedId>();

	BalanceableVariablesInComposition = TMap<FString, TArray<FNamedId>>();

	ActiveBalancingPerComposition = TMap<FString, FString>();

	BalanceableVariableDataForBalancingInComposition = TMap<
		FString, TMap<FString, TArray<TSharedRef<FBalancingData>>>>();
}

const TDataModelFeedback<TArray<FNamedId>> FDataModel::GetFreeVariables()
{
	const auto DataCopy = TArray<FNamedId>(FreeVariables);
	return TDataModelFeedback<TArray<FNamedId>>::CreateSuccessFeedback(DataCopy);
}

const TDataModelFeedback<TArray<FString>> FDataModel::GetCompositionNames()
{
	TArray<FString> Result = TArray<FString>();
	BalanceableVariablesInComposition.GetKeys(Result);
	return TDataModelFeedback<TArray<FString>>::CreateSuccessFeedback(Result);
}

const TDataModelFeedback<TArray<FString>> FDataModel::GetBalancingNames(const FString& CompositionName)
{
	const auto BalancingMap = BalanceableVariableDataForBalancingInComposition.Find(CompositionName);
	if (BalancingMap == nullptr) return COMPOSITION_NOT_EXISTS_ERROR(TArray<FString>);
	TArray<FString> Result = TArray<FString>();
	BalancingMap->GetKeys(Result);
	return TDataModelFeedback<TArray<FString>>::CreateSuccessFeedback(Result);
}

const TDataModelFeedback<TArray<FBalancingData>> FDataModel::GetBalancingData(
	const FString& CompositionName,
	const FString& BalancingName)
{
	const auto BalancingMap = BalanceableVariableDataForBalancingInComposition.Find(CompositionName);
	if (BalancingMap == nullptr) return COMPOSITION_NOT_EXISTS_ERROR(TArray<FBalancingData>);
	const auto ResultPtr = BalancingMap->Find(BalancingName);
	if (ResultPtr == nullptr) return BALANCING_NOT_EXISTS_ERROR(TArray<FBalancingData>);
	auto Result = TArray<FBalancingData>();
	for (int i = 0; i < ResultPtr->Num(); ++i)
	{
		auto data = FBalancingData((*ResultPtr)[i].Get());
		Result.Add(data);
	}
	return TDataModelFeedback<TArray<FBalancingData>>::CreateSuccessFeedback(Result);
}

const TDataModelFeedback<FString> FDataModel::GetCurrentlyActiveBalancing(const FString& CompositionName)
{
	const auto ActiveBalancing = ActiveBalancingPerComposition.Find(CompositionName);
	if (ActiveBalancing == nullptr) return COMPOSITION_NOT_EXISTS_ERROR(FString);
	return TDataModelFeedback<FString>::CreateSuccessFeedback(*ActiveBalancing);
}

const TDataModelFeedback<bool> FDataModel::AddNewComposition(const FString& CompositionName)
{
	if (BalanceableVariableDataForBalancingInComposition.Contains(CompositionName))
		return COMPOSITION_ALREADY_EXISTS_ERROR(bool);

	BalanceableVariableDataForBalancingInComposition.Add(
		CompositionName,
		TMap<FString, TArray<TSharedRef<FBalancingData>>>());
	BalanceableVariablesInComposition.Add(CompositionName, TArray<FNamedId>());

	return TDataModelFeedback<bool>::CreateSuccessFeedback(
		true,
		"Added new empty Composition \"" + CompositionName + "\"");
}

const TDataModelFeedback<bool> FDataModel::AddNewBalancingTo(
	const FString& CompositionName,
	const FString& BalancingName)
{
	const auto BalancingMap = BalanceableVariableDataForBalancingInComposition.Find(CompositionName);
	if (BalancingMap == nullptr)
		return COMPOSITION_NOT_EXISTS_ERROR(bool);

	if (BalancingMap->Contains(BalancingName))
		return BALANCING_ALREADY_EXISTS_ERROR(bool);

	auto Array = TArray<TSharedRef<FBalancingData>>();

	// Fill Array with currently Active or set as currently active
	const auto CurrentlyActiveFb = GetCurrentlyActiveBalancing(CompositionName);
	if (CurrentlyActiveFb.Success())
	{
		// Copy Currently Active
		const auto CurrentlyActive = CurrentlyActiveFb.GetDataOrThrow();
		const auto CurrentlyActiveDataFb = GetBalancingData(CompositionName, CurrentlyActive);
		check(CurrentlyActiveDataFb.Success());
		const auto CurrentlyActiveData = CurrentlyActiveDataFb.GetDataPtrOrThrow();
		for (auto Data : *CurrentlyActiveData)
		{
			Array.Add(MakeShareable(new FBalancingData(Data)));
		}
	}
	else
	{
		const auto Fb = SetActiveBalancing(CompositionName, BalancingName);
		check(Fb.Success());
	}

	BalancingMap->Add(BalancingName, Array);
	return TDataModelFeedback<bool>::CreateSuccessFeedback(
		true,
		"Added new empty Balancing \"" + BalancingName + "\" in Composition \"" + CompositionName + "\" ");
}

const TDataModelFeedback<bool> FDataModel::AddNewBalanceableVariableTo(
	const FString& CompositionName,
	const FNamedId DefaultData)
{
	const TDataModelFeedback<bool> Result = CheckAndBindVariableTo(CompositionName, DefaultData);
	if(!Result.Success()) return Result;
	auto BalancingMap = BalanceableVariableDataForBalancingInComposition.Find(CompositionName);

	// Add Default Data to all balancings
	if (BalancingMap != nullptr)
	{
		TArray<FString> Keys = TArray<FString>();
		BalancingMap->GetKeys(Keys);
		for (auto Key : Keys)
		{
			auto var = new FBalancingData();
			var->NamedId = DefaultData;
			var->LowerBound = -5.0f;
			var->CurrentValue = 0.0f;
			var->UpperBound = 5.0f;
			BalancingMap->Find(Key)->Add(MakeShareable(var));
		}
	}

	return Result;
}

const TDataModelFeedback<bool> FDataModel::SetActiveBalancing(const FString& CompositionName,
                                                              const FString& BalancingName)
{
	const auto CurrentlyActiveBalancingFb = GetCurrentlyActiveBalancing(CompositionName);
	if (CurrentlyActiveBalancingFb.Success())
	{
		const auto CurrentlyActiveBalancing = CurrentlyActiveBalancingFb.GetDataOrThrow();
		if (CurrentlyActiveBalancing.Equals(BalancingName))
			return TDataModelFeedback<bool>::CreateSuccessFeedback(true, "Balancing is already active.");

		const auto BalancingFb = GetBalancingData(CompositionName, BalancingName);
		if (!BalancingFb.Success()) return BALANCING_NOT_EXISTS_ERROR(bool);

		ActiveBalancingPerComposition[CompositionName] = BalancingName;
	}
	else
	{
		ActiveBalancingPerComposition.Add(CompositionName, BalancingName);
	}
	return TDataModelFeedback<bool>::CreateSuccessFeedback(
		true, "Balancing\"" + BalancingName + "\" is now active.");
}

const TDataModelFeedback<FBalancingData> FDataModel::UpdateBalancingValue(
	const FString& CompositionName,
	const FString& BalancingName,
	const FNamedId Id,
	const float NewValue)
{
	//Check if Composition exists
	const auto BalancingMap = BalanceableVariableDataForBalancingInComposition.Find(CompositionName);
	if (BalancingMap == nullptr)
		return COMPOSITION_NOT_EXISTS_ERROR(FBalancingData);

	const auto Balancing = BalancingMap->Find(BalancingName);
	if (Balancing == nullptr)
		return BALANCING_NOT_EXISTS_ERROR(FBalancingData);
	if (Balancing->Num() <= 0)
		return VARIABLE_NOT_PRESENT_ERROR(FBalancingData);

	int Index = -1;
	TWeakPtr<FBalancingData> Data;
	for (int i = 0; i < Balancing->Num(); ++i)
	{
		const TSharedRef<FBalancingData> Tmp = (*Balancing)[i];
		if (Tmp->NamedId == Id)
		{
			Index = i;
			Data = Tmp;
			break;
		}
	}
	if (Index < 0)
		return VARIABLE_NOT_PRESENT_ERROR(FBalancingData);

	const TSharedPtr<FBalancingData> DataShared = Data.Pin();
	// ReSharper disable twice CppObjectMemberMightNotBeInitialized
	DataShared->CurrentValue = FMath::Max(FMath::Min(NewValue, DataShared->UpperBound), DataShared->LowerBound);

	return TDataModelFeedback<FBalancingData>::CreateSuccessFeedback(
		FBalancingData(*DataShared),
		"Variable \"" + Id.GetName() + "\" was updated to new value.");
}

const TDataModelFeedback<FBalancingData> FDataModel::UpdateBalancingLowerBound(
	const FString& CompositionName,
	const FString& BalancingName,
	const FNamedId Id,
	const float NewValue)
{
	//Check if Composition exists
	const auto BalancingMap = BalanceableVariableDataForBalancingInComposition.Find(CompositionName);
	if (BalancingMap == nullptr)
		return COMPOSITION_NOT_EXISTS_ERROR(FBalancingData);

	const auto Balancing = BalancingMap->Find(BalancingName);
	if (Balancing == nullptr)
		return BALANCING_NOT_EXISTS_ERROR(FBalancingData);
	if (Balancing->Num() <= 0)
		return VARIABLE_NOT_PRESENT_ERROR(FBalancingData);

	int Index = -1;
	TWeakPtr<FBalancingData> Data;
	for (int i = 0; i < Balancing->Num(); ++i)
	{
		const TSharedRef<FBalancingData> Tmp = (*Balancing)[i];
		if (Tmp->NamedId == Id)
		{
			Index = i;
			Data = Tmp;
			break;
		}
	}
	if (Index < 0)
		return VARIABLE_NOT_PRESENT_ERROR(FBalancingData);

	const TSharedPtr<FBalancingData> DataShared = Data.Pin();
	DataShared->LowerBound = NewValue;
	// ReSharper disable once CppSomeObjectMembersMightNotBeInitialized

	return TDataModelFeedback<FBalancingData>::CreateSuccessFeedback(
		FBalancingData(*DataShared),
		"Variable \"" + Id.GetName() + "\" was updated with new lower bound.");
}

const TDataModelFeedback<FBalancingData> FDataModel::UpdateBalancingUpperBound(
	const FString& CompositionName,
	const FString& BalancingName,
	const FNamedId Id,
	const float NewValue)
{
	//Check if Composition exists
	const auto BalancingMap = BalanceableVariableDataForBalancingInComposition.Find(CompositionName);
	if (BalancingMap == nullptr)
		return COMPOSITION_NOT_EXISTS_ERROR(FBalancingData);

	const auto Balancing = BalancingMap->Find(BalancingName);
	if (Balancing == nullptr)
		return BALANCING_NOT_EXISTS_ERROR(FBalancingData);
	if (Balancing->Num() <= 0)
		return VARIABLE_NOT_PRESENT_ERROR(FBalancingData);

	int Index = -1;
	TWeakPtr<FBalancingData> Data;
	for (int i = 0; i < Balancing->Num(); ++i)
	{
		const TSharedRef<FBalancingData> Tmp = (*Balancing)[i];
		if (Tmp->NamedId == Id)
		{
			Index = i;
			Data = Tmp;
			break;
		}
	}
	if (Index < 0)
		return VARIABLE_NOT_PRESENT_ERROR(FBalancingData);

	const TSharedPtr<FBalancingData> DataShared = Data.Pin();
	DataShared->UpperBound = NewValue;

	return TDataModelFeedback<FBalancingData>::CreateSuccessFeedback(
		FBalancingData(*DataShared),
		"Variable \"" + Id.GetName() + "\" was updated with new upper bound.");
}

const TDataModelFeedback<bool> FDataModel::RemoveBalancingFrom(
	const FString& CompositionName,
	const FString& BalancingName)
{
	// Check if Composition exists
	const auto BalancingMap = BalanceableVariableDataForBalancingInComposition.Find(CompositionName);
	if (BalancingMap == nullptr)
		return COMPOSITION_NOT_EXISTS_ERROR(bool);

	// Check if Balancing exits
	const auto Balancing = BalancingMap->Find(BalancingName);
	if (Balancing == nullptr)
		return BALANCING_NOT_EXISTS_ERROR(bool);

	// Check if Balancing exits
	if (BalancingMap->Num() == 1)
		return CANT_REMOVE_LAST_BALANCING(bool);

	const auto CurrentlyActiveBalancing = GetCurrentlyActiveBalancing(CompositionName);
	if (CurrentlyActiveBalancing.Success() && CurrentlyActiveBalancing.GetDataOrThrow().Equals(BalancingName))
		return CANT_REMOVE_ACTIVE_BALANCING(bool);

	BalancingMap->Remove(BalancingName);
	return TDataModelFeedback<bool>::CreateSuccessFeedback(
		true,
		"Removed Balancing \"" + BalancingName + "\" from Composition \"" + CompositionName + "\" ");
}

const TDataModelFeedback<bool> FDataModel::RemoveComposition(const FString& CompositionName)
{
	// Check if Composition exists
	const auto BalancingMap = BalanceableVariableDataForBalancingInComposition.Find(CompositionName);
	if (BalancingMap == nullptr)
		return COMPOSITION_NOT_EXISTS_ERROR(bool);

	// Move FeedVariables to FreeVariables
	const auto FreedVariables = BalanceableVariablesInComposition.Find(CompositionName);
	if(FreedVariables != nullptr)
	{
		for (const auto FreeVariable : *FreedVariables)
		{
			RemoveBalanceableVariableFrom(CompositionName, FreeVariable);
		}
	}

	// Delete Data
	BalanceableVariableDataForBalancingInComposition.Remove(CompositionName);
	BalanceableVariablesInComposition.Remove(CompositionName);
	ActiveBalancingPerComposition.Remove(CompositionName);

	return TDataModelFeedback<bool>::CreateSuccessFeedback(true);
}

const TDataModelFeedback<bool> FDataModel::RemoveBalanceableVariableFrom(
	const FString& CompositionName, const FNamedId Id)
{
	// Check if Composition exists
	const auto OccupiedVariables = BalanceableVariablesInComposition.Find(CompositionName);
	if (OccupiedVariables == nullptr)
		return COMPOSITION_NOT_EXISTS_ERROR(bool);

	const auto b = OccupiedVariables->Contains(Id);
	if (!b) return VARIABLE_NOT_BOUND_TO_COMPOSITION_ERROR(bool);

	const auto BalancingMap = BalanceableVariableDataForBalancingInComposition.Find(CompositionName);

	auto Keys = TArray<FString>();
	BalancingMap->GetKeys(Keys);
	for (auto BalancingKey : Keys)
	{
		BalancingMap->Find(BalancingKey)->RemoveAll([Id](const TSharedRef<FBalancingData> BalancingData)
		{
			return Id == BalancingData->NamedId;
		});
	}

	// Move Variable to freeVariables
	OccupiedVariables->Remove(Id);
	FreeVariables.Add(Id);

	return TDataModelFeedback<bool>::CreateSuccessFeedback(true,
	                                                       "Variable \"" + Id.GetName() +
	                                                       "\" was removed from Composition \"" + CompositionName +
	                                                       "\".\n And is now free to be bound again.");
}

const TDataModelFeedback<bool> FDataModel::CheckAndBindVariableTo(const FString& CompositionName,
                                                               const FNamedId DefaultData)
{
	//Check if Composition exists
	if (!BalanceableVariablesInComposition.Contains(CompositionName))
		return COMPOSITION_NOT_EXISTS_ERROR(bool);
	//Check if variable is available
	if (!FreeVariables.Contains(DefaultData))
		return VARIABLE_NOT_AVAILABLE(bool);

	//Check if there exists at least one balancing for the composition
	auto BalancingMap = BalanceableVariableDataForBalancingInComposition.Find(CompositionName);
	if (BalancingMap == nullptr || BalancingMap->Num() <= 0)
		return TDataModelFeedback<bool>::CreateErrorFeedback(
			EDataModelError::IdentifierNotPresentInData,
			"There exists no balancing for the composition\"" + CompositionName +
			"\".\n You need to create a Balancing for it before you can assign Variables to it.");

	// Move Id from free to composition
	FreeVariables.RemoveSingle(DefaultData);
	BalanceableVariablesInComposition.Find(CompositionName)->Add(DefaultData);


	return TDataModelFeedback<bool>::CreateSuccessFeedback(
		true,
		"Added new Variable \"" + DefaultData.GetName() + "\" to Composition \"" + CompositionName + "\" "
	);
}
