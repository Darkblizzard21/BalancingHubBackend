// Fill out your copyright notice in the Description page of Project Settings.


#include "DataModelLoadingHelper.h"
#include "Dom/JsonObject.h"
#include "DataStructs.h"

bool FDataModelLoadingHelper::LoadActiveBalancingPerComposition(
	const TSharedPtr<FJsonObject> RootObj,
	TMap<FString, FString>& OutMap)
{
	auto ActiveBalancingPerCompositionObj = RootObj->GetArrayField("ActiveBalancingPerComposition");
	for (const auto ValuePairObj : ActiveBalancingPerCompositionObj)
	{
		const TSharedPtr<FJsonObject>* ActiveBalancingObj;
		const bool bSuccess = ValuePairObj->TryGetObject(ActiveBalancingObj);
		if (!bSuccess)
		{
			UE_LOG(LogTemp, Error, TEXT("BalancingHubSerialization: Failed to load ActiveBalancingPerComposition"));
			return false;
		}

		const FString Composition = ActiveBalancingObj->Get()->GetStringField(TEXT("Composition"));
		const FString Balancing = ActiveBalancingObj->Get()->GetStringField(TEXT("Balancing"));

		OutMap.Add(Composition, Balancing);
	}
	return true;
}

bool FDataModelLoadingHelper::LoadBalanceableVariableDataForBalancingInComposition(
	const TSharedPtr<FJsonObject> RootObj,
	TMap<FString, TMap<FString, TArray<TSharedRef<FBalancingData>>>>& OutMap)
{
	const auto ActiveBalancingPerCompositionObj =
		RootObj->GetArrayField("BalanceableVariableDataForBalancingInComposition");

	for (const auto CompositionJValue : ActiveBalancingPerCompositionObj)
	{
		const TSharedPtr<FJsonObject>* CompositionJObj;
		bool bSuccess = CompositionJValue->TryGetObject(CompositionJObj);
		if (!bSuccess)
		{
			UE_LOG(LogTemp, Error,
			       TEXT(
				       "BalancingHubSerialization: Failed to load a Composition in BalanceableVariableDataForBalancingInComposition"
			       ));
			return false;
		}
		const FString CompositionName = CompositionJObj->Get()->GetStringField(TEXT("CompositionName"));

		TArray<TSharedPtr<FJsonValue>> BalancingsJValueArray = CompositionJObj->Get()->GetArrayField(TEXT("Balancings"));
		TMap<FString, TArray<TSharedRef<FBalancingData>>> Balancings =
			TMap<FString, TArray<TSharedRef<FBalancingData>>>();

		for (const auto BalancingJValue : BalancingsJValueArray)
		{
			const TSharedPtr<FJsonObject>* BalancingJObj;
			bSuccess = BalancingJValue->TryGetObject(BalancingJObj);
			if (!bSuccess)
			{
				UE_LOG(LogTemp, Error,
					TEXT(
						"BalancingHubSerialization: Failed to load a Balancing in BalanceableVariableDataForBalancingInComposition"
					));
				return false;
			}
			
			const FString BalancingName = BalancingJObj->Get()->GetStringField(TEXT("BalancingName"));
			TArray<TSharedPtr<FJsonValue>> BDataJValueArray = BalancingJObj->Get()->GetArrayField(TEXT("CurrentValues"));
			TArray<TSharedRef<FBalancingData>> BData =
				TArray<TSharedRef<FBalancingData>>();

			for (const auto DataJValue : BDataJValueArray)
			{
				const TSharedPtr<FJsonObject>* DataJObj;
				bSuccess = DataJValue->TryGetObject(DataJObj);
				if (!bSuccess)
				{
					UE_LOG(LogTemp, Error,
						TEXT(
							"BalancingHubSerialization: Failed to load a BalanceableVariableData in BalanceableVariableDataForBalancingInComposition"
						));
					return false;
				}
				const FString Name = DataJObj->Get()->GetStringField(TEXT("Name"));
				const FString Id = DataJObj->Get()->GetStringField(TEXT("Id"));
				const float CurrentValue = DataJObj->Get()->GetNumberField(TEXT("CurrentValue"));
				const float LowerBound = DataJObj->Get()->GetNumberField(TEXT("LowerBound"));
				const float UpperBound = DataJObj->Get()->GetNumberField(TEXT("UpperBound"));

				auto Data = new FBalancingData();
                Data->NamedId = FNamedId::NewFNamedId(Name, Id);
                Data->LowerBound = LowerBound;
                Data->CurrentValue = CurrentValue;
                Data->UpperBound = UpperBound;
				BData.Add(MakeShareable(Data));
			}
			Balancings.Add(BalancingName, BData);
		}
		OutMap.Add(CompositionName,Balancings);
	}

	return true;
}

bool FDataModelLoadingHelper::LoadBalanceableVariablesInComposition(
	const TSharedPtr<FJsonObject> RootObj,
	TMap<FString, TArray<FNamedId>>& OutMap)
{
	const auto BalanceableVariablesInCompositionObj =
			RootObj->GetArrayField("BalanceableVariablesInComposition");
	
	for(const auto CompositionJValue : BalanceableVariablesInCompositionObj)
	{
		const TSharedPtr<FJsonObject>* CompositionJObj;
		bool bSuccess = CompositionJValue->TryGetObject(CompositionJObj);
		if (!bSuccess)
		{
			UE_LOG(LogTemp, Error,
				TEXT(
					"BalancingHubSerialization: Failed to load a Composition in BalanceableVariablesInComposition"
				));
			return false;
		}
		const FString CompositionName = CompositionJObj->Get()->GetStringField(TEXT("CompositionName"));

		TArray<TSharedPtr<FJsonValue>> BoundVariablesJValueArray =
			CompositionJObj->Get()->GetArrayField(TEXT("BoundVariables"));
		TArray<FNamedId>BoundVariables = TArray<FNamedId>();
		for (const auto BoundVariableJValue : BoundVariablesJValueArray)
		{
			const TSharedPtr<FJsonObject>*  BoundVariableJObj;
			bSuccess = BoundVariableJValue->TryGetObject(BoundVariableJObj);
			if (!bSuccess)
			{
				UE_LOG(LogTemp, Error,
					TEXT(
						"BalancingHubSerialization: Failed to load a BoundVariablee in BalanceableVariablesInComposition"
					));
				return false;
			}
			const FString Name = BoundVariableJObj->Get()->GetStringField(TEXT("Name"));
			const FString Id = BoundVariableJObj->Get()->GetStringField(TEXT("Id"));
			BoundVariables.Add(FNamedId::NewFNamedId(Name, Id));
		}
		OutMap.Add(CompositionName, BoundVariables);
	}
	return true;
}
