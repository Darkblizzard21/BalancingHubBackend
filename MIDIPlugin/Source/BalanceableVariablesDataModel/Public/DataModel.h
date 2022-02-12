// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "IDataModel.h"

#if !WITH_EDITORONLY_DATA
#define BALANCEABLEVARIABLESDATAMODEL_API 
#endif
/**
 * 
 */
class BALANCEABLEVARIABLESDATAMODEL_API FDataModel : public IIDataModel
{

public:
	FDataModel();

	// Begin: Information Getter

	virtual const TDataModelFeedback<TArray<FNamedId>> GetFreeVariables() override;
	virtual const TDataModelFeedback<TArray<FString>> GetCompositionNames() override;
	virtual const TDataModelFeedback<TArray<FString>> GetBalancingNames(const FString& CompositionName) override;
	virtual const TDataModelFeedback<TArray<FBalancingData>> GetBalancingData(
		const FString& CompositionName,
		const FString& BalancingName) override;
	virtual const TDataModelFeedback<FString> GetCurrentlyActiveBalancing(const FString& CompositionName) override;
	// End: Information Getter

	// Begin: DataAddRequests

	virtual const TDataModelFeedback<bool> AddNewComposition(const FString& CompositionName) override;
	virtual const TDataModelFeedback<bool> AddNewBalancingTo(
		const FString& CompositionName,
		const FString& BalancingName) override;
	virtual const TDataModelFeedback<bool> AddNewBalanceableVariableTo(
		const FString& CompositionName,
		const FNamedId DefaultData) override;
	// End: DataAddRequests

	// Begin: DataUpdateRequests

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
	// End: DataUpdateRequests

	// Begin: DataDeleteRequests

	virtual const TDataModelFeedback<bool> RemoveBalancingFrom(
		const FString& CompositionName,
		const FString& BalancingName) override;
	virtual const TDataModelFeedback<bool> RemoveComposition(const FString& CompositionName) override;
	virtual const TDataModelFeedback<bool> RemoveBalanceableVariableFrom(
		const FString& CompositionName,
		const FNamedId Id) override;
	// End: DataDeleteRequests

protected:
	// Variable Ownership Dat
	TArray<FNamedId> FreeVariables;
	
	TMap<FString, TArray<FNamedId>> BalanceableVariablesInComposition;

	// Currently active balancings
	
	TMap<FString, FString> ActiveBalancingPerComposition;

	// Balancing "DataBase" contains all Compositions with all their balancings and their FBalancingData
	TMap<FString,
	     TMap<
		     FString,
		     TArray<TSharedRef<FBalancingData>>
	     >
	>
	BalanceableVariableDataForBalancingInComposition;

protected:
	const TDataModelFeedback<bool> CheckAndBindVariableTo(
		const FString& CompositionName,
		const FNamedId DefaultData);
};
