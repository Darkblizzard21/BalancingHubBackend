#pragma once

#include "DataModel.h"
#include "BalanceableVariablesController.h"

class FJsonObject;

class FDataModelBackendLinked final : public FDataModel, public FNewVariablesPushReceiver
{
public:
	FDataModelBackendLinked();
	virtual const TDataModelFeedback<bool> AddNewBalanceableVariableTo(
		const FString& CompositionName,
		const FNamedId DefaultData) override;
	virtual const TDataModelFeedback<bool> SetActiveBalancing(
		const FString& CompositionName,
		const FString& BalancingName) override;
	virtual const TDataModelFeedback<FBalancingData> UpdateBalancingValue(
		const FString& CompositionName,
		const FString& BalancingName,
		const FNamedId Id,
		float NewValue) override;
	virtual const TDataModelFeedback<FBalancingData> UpdateBalancingLowerBound(
		const FString& CompositionName,
		const FString& BalancingName,
		const FNamedId Id,
		float NewValue) override;
	virtual const TDataModelFeedback<FBalancingData> UpdateBalancingUpperBound(
		const FString& CompositionName,
		const FString& BalancingName,
		const FNamedId Id,
		float NewValue) override;


	virtual void HandleNewData(FBalanceableVariableInfo Data) override;

	// Begin: Serialization
	bool LoadedSuccessfully() const {return bSuccessfullyLoaded;}
	void SaveDataToDisk();
private:
	bool LoadDataFromDisk();
	bool bSuccessfullyLoaded;
	
	void SaveActiveBalancingPerComposition(TSharedPtr<FJsonObject> RootObj);
	void SaveBalanceableVariableDataForBalancingInComposition(TSharedPtr<FJsonObject> RootObj);
	void SaveBalanceableVariablesInComposition(TSharedPtr<FJsonObject> RootObj);
	// End: Serialization
};
