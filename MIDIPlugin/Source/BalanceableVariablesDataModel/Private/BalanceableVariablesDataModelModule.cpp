// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "BalanceableVariablesDataModelModule.h"

#include "BalanceableVariablesController.h"
#include "DataModelBackendLinked.h"

#define LOCTEXT_NAMESPACE "FBalanceableVariablesDataModelModule"

void FBalanceableVariablesDataModelModule::StartupModule()
{
	if (bInit) return;
	DataModel = MakeShareable(new FDataModelBackendLinked());
	UBalanceableVariablesController::BindReceiver(DataModel);

	if (DataModel.Get()->LoadedSuccessfully())
	{
		const TDataModelFeedback<TArray<FString>> CompositionFeedback = DataModel.Get()->GetCompositionNames();
		TArray<FString> Compositions = CompositionFeedback.GetDataOrThrow();
		for (const auto Composition : Compositions)
		{
			TDataModelFeedback<FString> ActiveBalancingFb = DataModel.Get()->GetCurrentlyActiveBalancing(Composition);
			FString ActiveBalancing = ActiveBalancingFb.GetDataOrThrow();
			TDataModelFeedback<TArray<FBalancingData>> BalancingDataFeedback =
				DataModel.Get()->GetBalancingData(Composition, ActiveBalancing);
			TArray<FBalancingData> BalancingData = BalancingDataFeedback.GetDataOrThrow();
			for (const auto Data : BalancingData)
			{
				UBalanceableVariablesController::InjectOrOverrideBalanceableFloat(
					Data.NamedId.GetName(),
					Data.NamedId.GetId(),
					Data.CurrentValue
				);
			}
		}
	}

	//Override 
	bInit = true;
}

void FBalanceableVariablesDataModelModule::ShutdownModule()
{
	if (!bInit) return;
	DataModel->SaveDataToDisk();
	bInit = false;
}

TWeakPtr<IIDataModel> FBalanceableVariablesDataModelModule::GetDataModelInstance() const
{
	if (!bInit) throw "Module Not Instantiated";
	return DataModel;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBalanceableVariablesDataModelModule, BalanceableVariablesDataModel)
