// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DataModelFeedback.h"
#include "DataStructs.h"
#include "UObject/Interface.h"
#include "IDataModel.generated.h"

// This class does not need to be modified.
UINTERFACE()
class BALANCEABLEVARIABLESDATAMODEL_API UIDataModel : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BALANCEABLEVARIABLESDATAMODEL_API IIDataModel
{
	GENERATED_BODY()

public:
	// Begin: Information Getter

	virtual const TDataModelFeedback<TArray<FNamedId>> GetFreeVariables() = 0;
	virtual const TDataModelFeedback<TArray<FString>> GetCompositionNames() = 0;
	virtual const TDataModelFeedback<TArray<FString>> GetBalancingNames(const FString& CompositionName) = 0;
	virtual const TDataModelFeedback<TArray<FBalancingData>> GetBalancingData(
		const FString& CompositionName,
		const FString& BalancingName) = 0;
	virtual const TDataModelFeedback<FString> GetCurrentlyActiveBalancing(const FString& CompositionName) = 0;
	// End: Information Getter

	// Begin: DataAddRequests

	virtual const TDataModelFeedback<bool> AddNewComposition(const FString& CompositionName) = 0;
	virtual const TDataModelFeedback<bool> AddNewBalancingTo(
		const FString& CompositionName,
		const FString& BalancingName) = 0;
	virtual const TDataModelFeedback<bool> AddNewBalanceableVariableTo(
		const FString& CompositionName,
		const FNamedId DefaultData) = 0;
	// End: DataAddRequests

	// Begin: DataUpdateRequests
	virtual const TDataModelFeedback<bool> SetActiveBalancing(
		const FString& CompositionName,
		const FString& BalancingName) = 0;
	virtual const TDataModelFeedback<FBalancingData> UpdateBalancingValue(
		const FString& CompositionName,
		const FString& BalancingName,
		const FNamedId Id,
		float NewValue) = 0;
	virtual const TDataModelFeedback<FBalancingData> UpdateBalancingLowerBound(
		const FString& CompositionName,
		const FString& BalancingName,
		const FNamedId Id,
		float NewValue) = 0;
	virtual const TDataModelFeedback<FBalancingData> UpdateBalancingUpperBound(
		const FString& CompositionName,
		const FString& BalancingName,
		const FNamedId Id,
		float NewValue) = 0;
	// End: DataUpdateRequests

	// Begin: DataDeleteRequests

	virtual const TDataModelFeedback<bool> RemoveBalancingFrom(
		const FString& CompositionName,
		const FString& BalancingName) = 0;
	virtual const TDataModelFeedback<bool> RemoveComposition(const FString& CompositionName) = 0;
	virtual const TDataModelFeedback<bool> RemoveBalanceableVariableFrom(
		const FString& CompositionName,
		const FNamedId Id) = 0;
	// End: DataDeleteRequests
};
