// Copyright Pirmin Pfeifer. All Rights Reserved.

#include "DataModel.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDataModelTestsBalancingCreation,
                                 "GameMixerTests.DataModel.Balancing.CreationTest",
                                 EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::ProductFilter)

bool FDataModelTestsBalancingCreation::RunTest(const FString& Parameters)
{
	TSharedPtr<IIDataModel> DataModel = MakeShareable(new FDataModel());
	const FString EphemeralDataCompositionName = "EphemeralData";
	const FString FutureDataCompositionName = "FutureData";
	const FString GhostsBalancingName = "Ghosts";
	const FString WraithsBalancingName = "Wraiths";

	DataModel->AddNewComposition(EphemeralDataCompositionName);
	DataModel->AddNewComposition(FutureDataCompositionName);

	// Run Test

	// 1. Test: Add Balancing "Ghosts" & "Wraiths" to composition "EphemeralData"

	const TDataModelFeedback<bool> GhostsFeedback1 = DataModel->AddNewBalancingTo(
		EphemeralDataCompositionName, GhostsBalancingName);
	const TDataModelFeedback<bool> WraithsFeedback1 = DataModel->AddNewBalancingTo(
		EphemeralDataCompositionName, WraithsBalancingName);
	TestTrue("Check if \"Ghosts\" feedback was successful", GhostsFeedback1.Success());
	TestTrue("Check if \"Wraiths\" feedback was successful", WraithsFeedback1.Success());

	const TDataModelFeedback<TArray<FString>> EphemeralDataBalancingsFeedback = DataModel->GetBalancingNames(
		EphemeralDataCompositionName);
	TestTrue("Check if \"GetBalancingNames()\" feedback was successful", EphemeralDataBalancingsFeedback.Success());
	const TArray<FString> EphemeralDataBalancingsNames = EphemeralDataBalancingsFeedback.GetDataOrThrow();
	TestTrue("Check if the Feedback contains all Balancings: \"Ghosts\"",
	         EphemeralDataBalancingsNames.Contains("Ghosts"));
	TestTrue("Check if the Feedback contains all Balancings: \"Wraiths\"",
	         EphemeralDataBalancingsNames.Contains("Wraiths"));

	// 2. Test: If error feedback is given when trying to add same balancing again
	const TDataModelFeedback<bool> GhostsErrorFeedback = DataModel->AddNewBalancingTo(
		EphemeralDataCompositionName, GhostsBalancingName);
	TestFalse("Check if the GhostsErrorFeedback is failed.", GhostsErrorFeedback.Success());
	TestTrue("Check if the GhostsErrorFeedback is has the right error.",
	         GhostsErrorFeedback.GetError() == EDataModelError::IdentifierAlreadyExists);
	TestTrue(
		"Check if the GhostsErrorFeedback is has the right error message. Error Msg: " + GhostsErrorFeedback.
		GetMessage(),
		GhostsErrorFeedback.GetMessage().Equals("Balancing \"" + GhostsBalancingName + "\" already exists."));

	// 3. Test: Check if the same balancings can be added to another composition
	const TDataModelFeedback<bool> GhostsFeedback2 = DataModel->AddNewBalancingTo(
		FutureDataCompositionName, GhostsBalancingName);
	const TDataModelFeedback<bool> WraithsFeedback2 = DataModel->AddNewBalancingTo(
		FutureDataCompositionName, WraithsBalancingName);
	TestTrue("Check if \"Ghosts\" feedback was successful", GhostsFeedback2.Success());
	TestTrue("Check if \"Wraiths\" feedback was successful", WraithsFeedback2.Success());

	const TDataModelFeedback<TArray<FString>> FutureDataBalancingsFeedback = DataModel->GetBalancingNames(
		FutureDataCompositionName);
	TestTrue("Check if \"GetBalancingNames()\" feedback was successful", FutureDataBalancingsFeedback.Success());
	const TArray<FString> FutureDataBalancingsNames = FutureDataBalancingsFeedback.GetDataOrThrow();
	TestTrue("Check if the Feedback contains all Balancings: \"Ghosts\"", FutureDataBalancingsNames.Contains("Ghosts"));
	TestTrue("Check if the Feedback contains all Balancings: \"Wraiths\"",
	         FutureDataBalancingsNames.Contains("Wraiths"));

	// Deconstruct Everything
	// SharedPtr Delete them self

	// Return true to finish your test
	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDataModelTestsBalancingDeletion,
                                 "GameMixerTests.DataModel.Balancing.RemoveTest",
                                 EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::ProductFilter)

bool FDataModelTestsBalancingDeletion::RunTest(const FString& Parameters)
{
	TSharedPtr<IIDataModel> DataModel = MakeShareable(new FDataModel());
	const FString EphemeralDataCompositionName = "EphemeralData";
	const FString WraithsBalancingName = "Wraiths";
	const FString GhostsBalancingName = "Ghosts";

	DataModel->AddNewComposition(EphemeralDataCompositionName);
	DataModel->AddNewBalancingTo(EphemeralDataCompositionName, WraithsBalancingName);
	DataModel->AddNewBalancingTo(EphemeralDataCompositionName, GhostsBalancingName);

	// 1. Test: Delete Balancing "Ghosts"
	const TDataModelFeedback<bool> GhostsDeletionFeedback = DataModel->RemoveBalancingFrom(
		EphemeralDataCompositionName, GhostsBalancingName);
	TestTrue("Check if the Deletion of existing balancing is successful.", GhostsDeletionFeedback.Success());
	TestTrue("Check if the Deletion of existing balancing Feedback is contains true.",
	         GhostsDeletionFeedback.GetDataOrThrow());

	// 2. Test try to Delete Balancing "Ghosts" again
	const TDataModelFeedback<bool> GhostsDeletionFailFeedback = DataModel->RemoveBalancingFrom(
		EphemeralDataCompositionName, GhostsBalancingName);
	TestFalse("Check if the SteelDeletionFeedback2 is failed.", GhostsDeletionFailFeedback.Success());
	TestTrue("Check if the SteelDeletionFeedback2 is has the right error.",
	         GhostsDeletionFailFeedback.GetError() == EDataModelError::IdentifierNotPresentInData);
	TestTrue("Check if the InitialCreationFeedback is has the right error message.",
	         GhostsDeletionFailFeedback.GetMessage().
	                                    Equals("Balancing \"" + GhostsBalancingName + "\"dose not exists."));


	// 3. Test: Delete last Balancing "Wraiths"
	const TDataModelFeedback<bool> WraithsDeletionRejectFeedback = DataModel->RemoveBalancingFrom(
		EphemeralDataCompositionName, WraithsBalancingName);
	TestFalse("Check if the WraithsDeletionRejectFeedback is failed.", WraithsDeletionRejectFeedback.Success());
	TestTrue("Check if the WraithsDeletionRejectFeedback is has the right error.",
	         WraithsDeletionRejectFeedback.GetError() == EDataModelError::CantRemoveElement);
	TestTrue("Check if the InitialCreationFeedback is has the right error message.",
	         WraithsDeletionRejectFeedback.GetMessage().Equals(
		         "Balancing \"" + WraithsBalancingName + "\" is the last balancing of the Composition.\"" +
		         EphemeralDataCompositionName +
		         "\". \n Remove the composition instead."));


	// 4. Test: Add "Ghosts" after deleting it	// 3. Test: Add Composition "Steel" after deleting it
	const TDataModelFeedback<bool> GhostsRecreationFeedback = DataModel->AddNewComposition(GhostsBalancingName);
	TestTrue("Check if the GhostsRecreationFeedback is successful.", GhostsRecreationFeedback.Success());
	TestTrue("Check if the GhostsRecreationFeedback is contains true.", GhostsRecreationFeedback.GetDataOrThrow());

	// Deconstruct Everything
	// SharedPtr Delete them self


	// Return true to finish your test
	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDataModelTestsBalancingActiveBalancing,
                                 "GameMixerTests.DataModel.Balancing.ActiveBalancingTest",
                                 EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::ProductFilter)

bool FDataModelTestsBalancingActiveBalancing::RunTest(const FString& Parameters)
{
	TSharedPtr<IIDataModel> DataModel = MakeShareable(new FDataModel());
	const FString EphemeralDataCompositionName = "EphemeralData";
	const FString WraithsBalancingName = "Wraiths";
	const FString GhostsBalancingName = "Ghosts";

	DataModel->AddNewComposition(EphemeralDataCompositionName);
	DataModel->AddNewBalancingTo(EphemeralDataCompositionName, WraithsBalancingName);
	DataModel->AddNewBalancingTo(EphemeralDataCompositionName, GhostsBalancingName);

	// 1. Test: First added element "Wraiths" is default active
	const TDataModelFeedback<FString> DefaultActiveFeedback = DataModel->GetCurrentlyActiveBalancing(
		EphemeralDataCompositionName);
	TestTrue("Check if the GetCurrentlyActiveBalancing is successful.", DefaultActiveFeedback.Success());
	TestTrue("Check if the DefaultActiveFeedback is contains Wraith.",
	         DefaultActiveFeedback.GetDataOrThrow().Equals(WraithsBalancingName));

	// 2. Test: Cont remove active balancing
	const TDataModelFeedback<bool> RemoveActiveRejectFeedback = DataModel->RemoveBalancingFrom(
		EphemeralDataCompositionName, WraithsBalancingName);
	TestFalse("Check if the WraithsDeletionRejectFeedback is failed.", RemoveActiveRejectFeedback.Success());
	TestTrue("Check if the WraithsDeletionRejectFeedback is has the right error.",
	         RemoveActiveRejectFeedback.GetError() == EDataModelError::CantRemoveElement);
	TestTrue("Check if the InitialCreationFeedback is has the right error message.",
	         RemoveActiveRejectFeedback.GetMessage().Equals(
		         "Balancing \"" + WraithsBalancingName + "\" is the active balancing of the Composition.\"" +
		         EphemeralDataCompositionName +
		         "\". \n Set Another as active before removal."));


	// 3. Test: Change Active balancing to "Ghosts"
	const TDataModelFeedback<bool> ChangeActiveFeedback = DataModel->SetActiveBalancing(
		EphemeralDataCompositionName, GhostsBalancingName);
	TestTrue("Check if SetActiveBalancing is successful.", ChangeActiveFeedback.Success());
	TestTrue("Check if SetActiveBalancing Feedback contains true.", ChangeActiveFeedback.GetDataOrThrow());
	const TDataModelFeedback<FString> AfterChangeActiveFeedback = DataModel->GetCurrentlyActiveBalancing(
		EphemeralDataCompositionName);
	TestTrue("Check if the GetCurrentlyActiveBalancing after a change is successful.", AfterChangeActiveFeedback.Success());
	TestTrue("Check if the AfterChangeActiveFeedback is contains Ghosts.",
			AfterChangeActiveFeedback.GetDataOrThrow().Equals(GhostsBalancingName));

	// Deconstruct Everything
	// SharedPtr Delete them self


	// Return true to finish your test
	return true;
}
