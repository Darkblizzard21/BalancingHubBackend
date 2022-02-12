// Copyright Pirmin Pfeifer. All Rights Reserved.

#include "DataModel.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDataModelTestsCompositionCreation,
                                 "GameMixerTests.DataModel.Composition.CreationTest",
                                 EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::ProductFilter)

bool FDataModelTestsCompositionCreation::RunTest(const FString& Parameters)
{
	TSharedPtr<IIDataModel> DataModel = MakeShareable(new FDataModel());
	// Run Test

	// 1. Test: Add Composition "Steel"
	const TDataModelFeedback<bool> SteelCreationFeedback = DataModel->AddNewComposition("Steel");
	TestTrue("Check if the InitialCreationFeedback is successful.", SteelCreationFeedback.Success());
	TestTrue("Check if the InitialCreationFeedback is contains true.", SteelCreationFeedback.GetDataOrThrow());

	// 2. Test try to add Composition "Steel" again
	const TDataModelFeedback<bool> SteelCreationFeedback2 = DataModel->AddNewComposition("Steel");
	TestFalse("Check if the SteelCreationFeedback2 is failed.", SteelCreationFeedback2.Success());
	TestTrue("Check if the InitialCreationFeedback is has the right error.",
	         SteelCreationFeedback2.GetError() == EDataModelError::IdentifierAlreadyExists);
	TestTrue(
		"Check if the InitialCreationFeedback is has the right error message. Error Msg: " + SteelCreationFeedback2.
		GetMessage(),
		SteelCreationFeedback2.GetMessage().Equals("Composition \"Steel\" already exists"));


	// Deconstruct Everything
	// SharedPtr Delete them self

	// Return true to finish your test
	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDataModelTestsCompositionDeletion,
                                 "GameMixerTests.DataModel.Composition.RemovingTest",
                                 EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::ProductFilter)

bool FDataModelTestsCompositionDeletion::RunTest(const FString& Parameters)
{
	TSharedPtr<IIDataModel> DataModel = MakeShareable(new FDataModel());
	DataModel->AddNewComposition("Steel");
	// Run Test

	// 1. Test: Delete Composition "Steel"
	const TDataModelFeedback<bool> SteelDeletionFeedback = DataModel->RemoveComposition("Steel");
	TestTrue("Check if the Deletion of existing composition is successful.", SteelDeletionFeedback.Success());
	TestTrue("Check if the Deletion of existing composition Feedback is contains true.",
	         SteelDeletionFeedback.GetDataOrThrow());

	// 2. Test try to Delete Composition "Steel" again
	const TDataModelFeedback<bool> SteelDeletionFeedback2 = DataModel->RemoveComposition("Steel");
	TestFalse("Check if the SteelDeletionFeedback2 is failed.", SteelDeletionFeedback2.Success());
	TestTrue("Check if the SteelDeletionFeedback2 is has the right error.",
	         SteelDeletionFeedback2.GetError() == EDataModelError::IdentifierNotPresentInData);
	TestTrue("Check if the InitialCreationFeedback is has the right error message.",
	         SteelDeletionFeedback2.GetMessage().Equals("Composition \"Steel\" dose not exists."));

	// 2.5. Test if Composition "Steel" is no longer in composition names
	const TDataModelFeedback<TArray<FString>> CompositionsAfterRemove = DataModel->GetCompositionNames();
	TestFalse("Check if the SteelDeletionFeedback2 is failed.", CompositionsAfterRemove.GetDataOrThrow().Contains("Steel"));
	
	// 3. Test: Add Composition "Steel" after deleting it
	const TDataModelFeedback<bool> SteelCreationFeedback = DataModel->AddNewComposition("Steel");
	TestTrue("Check if the InitialCreationFeedback is successful.", SteelCreationFeedback.Success());
	TestTrue("Check if the InitialCreationFeedback is contains true.", SteelCreationFeedback.GetDataOrThrow());

	// Deconstruct Everything
	// SharedPtr Delete them self

	// Return true to finish your test
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDataModelTestsCompositionGetComposititionNamesTest,
                                 "GameMixerTests.DataModel.Composition.GetCompositionNamesTest",
                                 EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::ProductFilter)

bool FDataModelTestsCompositionGetComposititionNamesTest::RunTest(const FString& Parameters)
{
	TSharedPtr<IIDataModel> DataModel = MakeShareable(new FDataModel());
	DataModel->AddNewComposition("Fire");
	DataModel->AddNewComposition("Blood");
	DataModel->AddNewComposition("Steel");
	// Run Test

	// 1. Test: Get CompositionNames
	// 1.1. Test: valid Feedback
	const TDataModelFeedback<TArray<FString>> FirstFeedback = DataModel->GetCompositionNames();
	TestTrue("Check if the FirstFeedback of CompositionNames() is successful", FirstFeedback.Success());
	try
	{
		const TArray<FString> Names = FirstFeedback.GetDataOrThrow();
	}
	catch (...)
	{
		TestTrue("FirstFeedback of CompositionNames() threw on FirstFeedback.GetDataOrThrow()", false);
	}
	// 1.2. Test: valid Feedback data
	const TArray<FString> Names = FirstFeedback.GetDataOrThrow();
	TestTrue("Check if the Feedback contains all compositions: \"Fire\"", Names.Contains("Fire"));
	TestTrue("Check if the Feedback contains all compositions: \"Blood\"", Names.Contains("Blood"));
	TestTrue("Check if the Feedback contains all compositions: \"Steel\"", Names.Contains("Steel"));


	// 2. Test: Data Independence from feedback
	const TDataModelFeedback<TArray<FString>> IndecencyFeedback1 = DataModel->GetCompositionNames();
	TArray<FString> IndependentData = IndecencyFeedback1.GetDataOrThrow();
	IndependentData.Add(FString("OddOneOut"));
	const TDataModelFeedback<TArray<FString>> IndecencyFeedback2 = DataModel->GetCompositionNames();
	TestFalse("Check if the Feedback contains all compositions: \"Fire\"",
	          IndecencyFeedback2.GetDataOrThrow().Contains("OddOneOut"));

	// Deconstruct Everything
	// SharedPtr Delete them self

	// Return true to finish your test
	return true;
}
