// Copyright Pirmin Pfeifer. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "TestDoubles/DataModelAccess.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDataModelTestsBalancingVaraiablesCreation,
                                 "GameMixerTests.DataModel.BalancingVariableData.CreationTest",
                                 EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::ProductFilter)

bool FDataModelTestsBalancingVaraiablesCreation::RunTest(const FString& Parameters)
{
	TSharedPtr<FDataModelAccess> DataModel = MakeShareable(new FDataModelAccess());
	const FString EphemeralDataCompositionName = "EphemeralData";
	const FString GhostsBalancingName = "Ghosts";
	const FString WraithsBalancingName = "Wraiths";
	DataModel->AddNewComposition(EphemeralDataCompositionName);
	DataModel->AddNewBalancingTo(EphemeralDataCompositionName, GhostsBalancingName);
	DataModel->AddNewBalancingTo(EphemeralDataCompositionName, WraithsBalancingName);

	const FNamedId Speed = FNamedId::NewFNamedId("Speed", "0");
	const FNamedId JumpHeight = FNamedId::NewFNamedId("JumpHeight", "1");
	TArray<FNamedId> FreeVariables = TArray<FNamedId>();
	FreeVariables.Add(Speed);
	FreeVariables.Add(JumpHeight);
	DataModel->OverrideFreeVariables(FreeVariables);
	// Run Test

	// 1. Test: Add Balancing "Speed" & "JumpHeight" to composition "EphemeralData"
	const TDataModelFeedback<bool> SpeedInitFeedback = DataModel->AddNewBalanceableVariableTo(
		EphemeralDataCompositionName, Speed);
	TestTrue("Check if SpeedInitFeedback was successful", SpeedInitFeedback.Success());

	const TDataModelFeedback<bool> JumpHeightInitFeedback = DataModel->AddNewBalanceableVariableTo(
		EphemeralDataCompositionName, JumpHeight);
	TestTrue("Check if JumpHeightInitFeedback was successful", JumpHeightInitFeedback.Success());


	const TDataModelFeedback<TArray<FBalancingData>> CurrentDataGhosts = DataModel->GetBalancingData(
		EphemeralDataCompositionName, GhostsBalancingName);
	TestTrue("Check if CurrentDataGhostsFB was successful", CurrentDataGhosts.Success());
	TestTrue("Check if CurrentDataGhostsFB contains 0. element",
	         CurrentDataGhosts.GetDataOrThrow().ContainsByPredicate([Speed](const FBalancingData Data)
	         {
		         return Data.NamedId == Speed;
	         }));
	TestTrue("Check if CurrentDataGhostsFB contains 1. element",
	         CurrentDataGhosts.GetDataOrThrow().ContainsByPredicate([JumpHeight](const FBalancingData Data)
	         {
		         return Data.NamedId == JumpHeight;
	         }));

	const TDataModelFeedback<TArray<FBalancingData>> CurrentDataWraiths = DataModel->GetBalancingData(
		EphemeralDataCompositionName, WraithsBalancingName);
	TestTrue("Check if CurrentDataWraithsFB was successful", CurrentDataWraiths.Success());
	TestTrue("Check if CurrentDataWraithsFB contains 0. element",
	         CurrentDataWraiths.GetDataOrThrow().ContainsByPredicate([Speed](const FBalancingData Data)
	         {
		         return Data.NamedId == Speed;
	         }));
	TestTrue("Check if CurrentDataWraithsFB contains 1. element",
	         CurrentDataWraiths.GetDataOrThrow().ContainsByPredicate([JumpHeight](const FBalancingData Data)
	         {
		         return Data.NamedId == JumpHeight;
	         }));


	const TArray<FNamedId> CurrentFreeVars = DataModel->GetFreeVariables().GetDataOrThrow();
	TestTrue("Check if FreeVariables are empty after binding all variables", CurrentFreeVars.Num() == 0);

	// 3. Test if variables get rejected if already bound
	const TDataModelFeedback<bool> ErrorFeedBackDoubleAddSpeed = DataModel->AddNewBalanceableVariableTo(
		EphemeralDataCompositionName, Speed);
	TestFalse("Check if adding already bound variable again is not successful.",
	          ErrorFeedBackDoubleAddSpeed.Success());
	TestTrue("Check if ErrorFeedBackDoubleAddSpeed contains right error",
	         ErrorFeedBackDoubleAddSpeed.GetError() == EDataModelError::IdentifierNotPresentInData);

	// Deconstruct Everything
	// SharedPtr Delete them self

	// Return true to finish your test
	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDataModelTestsBalancingVaraiablesUpdate,
                                 "GameMixerTests.DataModel.BalancingVariableData.UpdateValuesTest",
                                 EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::ProductFilter)

bool FDataModelTestsBalancingVaraiablesUpdate::RunTest(const FString& Parameters)
{
	TSharedPtr<FDataModelAccess> DataModel = MakeShareable(new FDataModelAccess());
	const FString EphemeralDataCompositionName = "EphemeralData";
	const FString GhostsBalancingName = "Ghosts";
	DataModel->AddNewComposition(EphemeralDataCompositionName);
	DataModel->AddNewBalancingTo(EphemeralDataCompositionName, GhostsBalancingName);

	const FNamedId Speed = FNamedId::NewFNamedId("Speed", "0");
	TArray<FNamedId> FreeVariables = TArray<FNamedId>();
	FreeVariables.Add(Speed);
	DataModel->OverrideFreeVariables(FreeVariables);

	DataModel->AddNewBalanceableVariableTo(EphemeralDataCompositionName, Speed);
	// Run Test

	// 1. Test UpdateBalancingValue
	const TDataModelFeedback<FBalancingData> Update1Feedback = DataModel->UpdateBalancingValue(
		EphemeralDataCompositionName, GhostsBalancingName, Speed, 0.5f);
	TestTrue("Check if update of balancing variables works.", Update1Feedback.Success());
	TestTrue("Check if update of balancing variables works.",
	         Update1Feedback.GetDataOrThrow().CurrentValue == 0.5f);

	// 2. Test UpdateBalancingValue with data bigger than max value
	const TDataModelFeedback<FBalancingData> Update2Feedback = DataModel->UpdateBalancingValue(
		EphemeralDataCompositionName, GhostsBalancingName, Speed,
		Update1Feedback.GetDataOrThrow().UpperBound + 10.f);
	TestTrue("Check if bounds are enforced works.", Update2Feedback.Success());
	TestTrue("Check if bounds are enforced works.",
	         Update2Feedback.GetDataOrThrow().CurrentValue == Update2Feedback.GetDataOrThrow().UpperBound);

	// 3. Test UpdateBalancingValue with data smaller than min value
	const TDataModelFeedback<FBalancingData> Update3Feedback = DataModel->UpdateBalancingValue(
		EphemeralDataCompositionName, GhostsBalancingName, Speed,
		Update2Feedback.GetDataOrThrow().LowerBound - 10.f);
	TestTrue("Check if bounds are enforced works.", Update2Feedback.Success());
	TestTrue("Check if bounds are enforced works with exlicit values.",
	         Update3Feedback.GetDataOrThrow().CurrentValue == Update3Feedback.GetDataOrThrow().LowerBound);

	// 4. Update lower bound
	const TDataModelFeedback<FBalancingData> Update4Feedback = DataModel->UpdateBalancingLowerBound(
		EphemeralDataCompositionName, GhostsBalancingName, Speed, -100.0f);
	TestTrue("Check if update of balancing variables lower bound works.", Update4Feedback.Success());
	TestTrue("Check if update of balancing variables lower value is correctly update.",
	         FMath::IsNearlyEqual(Update4Feedback.GetDataOrThrow().LowerBound, -100.0f));

	// 5. Update upper bound
	const TDataModelFeedback<FBalancingData> Update5Feedback = DataModel->UpdateBalancingUpperBound(
		EphemeralDataCompositionName, GhostsBalancingName, Speed, 100.0f);
	TestTrue("Check if update of balancing variables upper bound works.", Update5Feedback.Success());
	TestTrue("Check if update of balancing variables upper bound value is correctly update.",
	         FMath::IsNearlyEqual(Update5Feedback.GetDataOrThrow().UpperBound, 100.0f));


	// Deconstruct Everything
	// SharedPtr Delete them self

	// Return true to finish your test
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDataModelTestsBalancingVaraiablesUpdateError,
                                 "GameMixerTests.DataModel.BalancingVariableData.UpdateErrorTest",
                                 EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::ProductFilter)

bool FDataModelTestsBalancingVaraiablesUpdateError::RunTest(const FString& Parameters)
{
	TSharedPtr<FDataModelAccess> DataModel = MakeShareable(new FDataModelAccess());
	const FString EphemeralDataCompositionName = "EphemeralData";
	const FString GhostsBalancingName = "Ghosts";
	DataModel->AddNewComposition(EphemeralDataCompositionName);
	DataModel->AddNewBalancingTo(EphemeralDataCompositionName, GhostsBalancingName);

	const FNamedId Speed = FNamedId::NewFNamedId("Speed", "0");
	TArray<FNamedId> FreeVariables = TArray<FNamedId>();
	// Run Test

	// 1. Non existent composition
	const TDataModelFeedback<FBalancingData> CompositionErrorValue = DataModel->UpdateBalancingValue(
		"null", "null", Speed, 0);
	TestFalse("Check if CompositionErrorValue is not successful.", CompositionErrorValue.Success());
	TestTrue("Check if CompositionErrorValue contains the right error.",
	         CompositionErrorValue.GetError() == EDataModelError::IdentifierNotPresentInData);

	const TDataModelFeedback<FBalancingData> CompositionErrorLowerBound = DataModel->UpdateBalancingLowerBound(
		"null", "null", Speed, 0);
	TestFalse("Check if CompositionErrorLowerBound is not successful.", CompositionErrorLowerBound.Success());
	TestTrue("Check if CompositionErrorLowerBound contains the right error.",
	         CompositionErrorLowerBound.GetError() == EDataModelError::IdentifierNotPresentInData);

	const TDataModelFeedback<FBalancingData> CompositionErrorUpperBound = DataModel->UpdateBalancingUpperBound(
		"null", "null", Speed, 0);
	TestFalse("Check if CompositionErrorLowerBound is not successful.", CompositionErrorUpperBound.Success());
	TestTrue("Check if CompositionErrorLowerBound contains the right error.",
	         CompositionErrorUpperBound.GetError() == EDataModelError::IdentifierNotPresentInData);

	// 2. Non existent balancing
	const TDataModelFeedback<FBalancingData> BalancingsErrorValue = DataModel->UpdateBalancingValue(
		EphemeralDataCompositionName, "null", Speed, 0);
	TestFalse("Check if BalancingsErrorValue is not successful.", BalancingsErrorValue.Success());
	TestTrue("Check if BalancingsErrorValue contains the right error.",
	         BalancingsErrorValue.GetError() == EDataModelError::IdentifierNotPresentInData);

	const TDataModelFeedback<FBalancingData> BalancingsErrorLowerBound = DataModel->UpdateBalancingLowerBound(
		EphemeralDataCompositionName, "null", Speed, 0);
	TestFalse("Check if BalancingsErrorLowerBound is not successful.", BalancingsErrorLowerBound.Success());
	TestTrue("Check if BalancingsErrorLowerBound contains the right error.",
	         BalancingsErrorLowerBound.GetError() == EDataModelError::IdentifierNotPresentInData);

	const TDataModelFeedback<FBalancingData> BalancingsErrorUpperBound = DataModel->UpdateBalancingUpperBound(
		EphemeralDataCompositionName, "null", Speed, 0);
	TestFalse("Check if BalancingsErrorUpperBound is not successful.", BalancingsErrorUpperBound.Success());
	TestTrue("Check if BalancingsErrorUpperBound contains the right error.",
	         BalancingsErrorUpperBound.GetError() == EDataModelError::IdentifierNotPresentInData);

	// 3. Non existent variable
	const TDataModelFeedback<FBalancingData> VariableErrorValue = DataModel->UpdateBalancingValue(
		EphemeralDataCompositionName, GhostsBalancingName, Speed, 0);
	TestFalse("Check if VariableErrorValue is not successful.", BalancingsErrorValue.Success());
	TestTrue("Check if VariableErrorValue contains the right error.",
	         BalancingsErrorValue.GetError() == EDataModelError::IdentifierNotPresentInData);

	const TDataModelFeedback<FBalancingData> VariableErrorLowerBound = DataModel->UpdateBalancingLowerBound(
		EphemeralDataCompositionName, GhostsBalancingName, Speed, 0);
	TestFalse("Check if VariableErrorLowerBound is not successful.", BalancingsErrorLowerBound.Success());
	TestTrue("Check if VariableErrorLowerBound contains the right error.",
	         BalancingsErrorLowerBound.GetError() == EDataModelError::IdentifierNotPresentInData);

	const TDataModelFeedback<FBalancingData> VariableErrorUpperBound = DataModel->UpdateBalancingUpperBound(
		EphemeralDataCompositionName, GhostsBalancingName, Speed, 0);
	TestFalse("Check if VariableErrorUpperBound is not successful.", BalancingsErrorUpperBound.Success());
	TestTrue("Check if VariableErrorUpperBound contains the right error.",
	         BalancingsErrorUpperBound.GetError() == EDataModelError::IdentifierNotPresentInData);


	// Deconstruct Everything
	// SharedPtr Delete them self

	// Return true to finish your test
	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDataModelTestsBalancingVaraiablesRemove,
                                 "GameMixerTests.DataModel.BalancingVariableData.RemovingTest",
                                 EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::ProductFilter)

bool FDataModelTestsBalancingVaraiablesRemove::RunTest(const FString& Parameters)
{
	TSharedPtr<FDataModelAccess> DataModel = MakeShareable(new FDataModelAccess());
	const FString EphemeralDataCompositionName = "EphemeralData";
	const FString GhostsBalancingName = "Ghosts";
	DataModel->AddNewComposition(EphemeralDataCompositionName);
	DataModel->AddNewBalancingTo(EphemeralDataCompositionName, GhostsBalancingName);

	const FNamedId Speed = FNamedId::NewFNamedId("Speed", "0");
	TArray<FNamedId> FreeVariables = TArray<FNamedId>();
	FreeVariables.Add(Speed);
	DataModel->OverrideFreeVariables(FreeVariables);

	DataModel->AddNewBalanceableVariableTo(EphemeralDataCompositionName, Speed);
	// Run Test

	// 1. Test: Remove Speed
	const TDataModelFeedback<bool> SpeedDeletionFeedback = DataModel->RemoveBalanceableVariableFrom(
		EphemeralDataCompositionName, Speed);
	TestTrue("Check if the the removing of bound variable is successful.", SpeedDeletionFeedback.Success());
	TestTrue("Check if the the removing of bound variable Feedback is contains true.",
	         SpeedDeletionFeedback.GetDataOrThrow());
	const TDataModelFeedback<TArray<FNamedId>> FreeVariablesAfterRemove = DataModel->GetFreeVariables();
	TestTrue("Check if GetFreeVariables after remove is successful.", FreeVariablesAfterRemove.Success());
	TestTrue("Check if FreeVariablesAfterRemove contain SpeedNamedId.",
	         FreeVariablesAfterRemove.GetDataOrThrow().Contains(Speed));

	// 2. Test: Remove Speed again
	const TDataModelFeedback<bool> SpeedDeletionErrorFeedback = DataModel->RemoveBalanceableVariableFrom(
		EphemeralDataCompositionName, Speed);
	TestFalse("Check if the SpeedDeletionErrorFeedback is failed.", SpeedDeletionErrorFeedback.Success());
	TestTrue("Check if the SpeedDeletionErrorFeedback is has the right error.",
	         SpeedDeletionErrorFeedback.GetError() == EDataModelError::IdentifierNotPresentInData);

	// Deconstruct Everything
	// SharedPtr Delete them self

	// Return true to finish your test
	return true;
}
