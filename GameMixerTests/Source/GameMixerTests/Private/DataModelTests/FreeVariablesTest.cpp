// Copyright Pirmin Pfeifer. All Rights Reserved.

#include "StaticVariables.h"
#include "Misc/AutomationTest.h"
#include "TestDoubles/DataModelAccess.h"
#include "Engine/Engine.h"


class FRuntimeTestDataModelConsistencyTest : public IAutomationLatentCommand
{
public:
	FRuntimeTestDataModelConsistencyTest(FAutomationTestBase* Test,
	                                     TSharedPtr<IIDataModel> DataModel) : Test(Test), DataModel(DataModel)
	{
		i = 0;
	}

	virtual ~FRuntimeTestDataModelConsistencyTest() override
	{
	}

	virtual bool Update() override;
private:
	FAutomationTestBase* Test;
	int i;
	TSharedPtr<IIDataModel> DataModel;
};

bool FRuntimeTestDataModelConsistencyTest::Update()
{
	switch (i)
	{
	case 1:
		GEngine->ForceGarbageCollection();
		break;

	case 6:
		try
		{
			const auto Fb = DataModel->GetFreeVariables();
			Test->TestTrue("Check if the DataModelFeedback is successful", Fb.Success());
			const auto DM_FreeVariables = Fb.GetDataOrThrow();
			Test->TestTrue("Check if length of FreeVariables Array is consistent",
			               DataModelTestVariables::FreeVariables.Num() == DM_FreeVariables.Num());
			Test->TestTrue("Check if first element is equal.",
			               DataModelTestVariables::FreeVariables[0] == (DM_FreeVariables)[0]);
			Test->TestTrue("Check if first element has same name.",
			               DataModelTestVariables::FreeVariables[0].GetName() == (DM_FreeVariables)[0].GetName());
			Test->TestTrue("Check if second element is equal.",
			               DataModelTestVariables::FreeVariables[1] == (DM_FreeVariables)[1]);
			Test->TestTrue("Check if second element has same name.",
			               DataModelTestVariables::FreeVariables[1].GetName() == (DM_FreeVariables)[1].GetName());
			Test->TestTrue("Check if third element is equal.",
			               DataModelTestVariables::FreeVariables[2] == (DM_FreeVariables)[2]);
			Test->TestTrue("Check if third element has same name.",
			               DataModelTestVariables::FreeVariables[2].GetName() == (DM_FreeVariables)[2].GetName());
			Test->TestTrue("Check if fourth element is equal.",
			               DataModelTestVariables::FreeVariables[3] == (DM_FreeVariables)[3]);
			Test->TestTrue("Check if fourth element has same name.",
			               DataModelTestVariables::FreeVariables[3].GetName() == (DM_FreeVariables)[3].GetName());
			Test->TestTrue("Check if fifth element is equal.",
			               DataModelTestVariables::FreeVariables[4] == (DM_FreeVariables)[4]);
			Test->TestTrue("Check if fifth element has same name.",
			               DataModelTestVariables::FreeVariables[4].GetName() == (DM_FreeVariables)[4].GetName());
		}
		catch (...)
		{
			Test->TestTrue("GetFreeVariables Start Content consistency Failed after 5 after garbage collection frames.",
			               false);
		}
		return true;
	default:
		break;
	}
	i++;
	return false;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDataModelTestsConsitencyTest,
                                 "GameMixerTests.DataModel.ConsitencyTest",
                                 EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::ProductFilter)

bool FDataModelTestsConsitencyTest::RunTest(const FString& Parameters)
{
	TSharedPtr<FDataModelAccess> DataModel = MakeShareable(new FDataModelAccess());
	DataModel->OverrideFreeVariables(DataModelTestVariables::FreeVariables);
	// Run Test
	// STart Consistency test
	FAutomationTestFramework::Get().EnqueueLatentCommand(
		MakeShareable(new FRuntimeTestDataModelConsistencyTest(this, DataModel)));

	// Deconstruct Everything

	// Return true to finish your test
	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDataModelTestsFreeVariables,
                                 "GameMixerTests.DataModel.GetFreeVariablesTests",
                                 EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::ProductFilter)

bool FDataModelTestsFreeVariables::RunTest(const FString& Parameters)
{
	TSharedPtr<FDataModelAccess> DataModel = MakeShareable(new FDataModelAccess());
	DataModel->OverrideFreeVariables(DataModelTestVariables::FreeVariables);
	// Run Test

	// 1. Test: GetFreeVariables Success
	const auto Fb = DataModel->GetFreeVariables();
	TestTrue("Check if the DataModelFeedback is successful", Fb.Success());

	// 2. Test: GetFreeVariables Start Content consistency
	try
	{
		const auto DM_FreeVariables = Fb.GetDataOrThrow();
		TestTrue("Check if length of FreeVariables Array is consistent",
		         DataModelTestVariables::FreeVariables.Num() == DM_FreeVariables.Num());
		TestTrue("Check if first element is equal.", DataModelTestVariables::FreeVariables[0] == (DM_FreeVariables)[0]);
		TestTrue("Check if first element has same name.",
		         DataModelTestVariables::FreeVariables[0].GetName() == (DM_FreeVariables)[0].GetName());
		TestTrue("Check if second element is equal.",
		         DataModelTestVariables::FreeVariables[1] == (DM_FreeVariables)[1]);
		TestTrue("Check if second element has same name.",
		         DataModelTestVariables::FreeVariables[1].GetName() == (DM_FreeVariables)[1].GetName());
		TestTrue("Check if third element is equal.", DataModelTestVariables::FreeVariables[2] == (DM_FreeVariables)[2]);
		TestTrue("Check if third element has same name.",
		         DataModelTestVariables::FreeVariables[2].GetName() == (DM_FreeVariables)[2].GetName());
		TestTrue("Check if fourth element is equal.",
		         DataModelTestVariables::FreeVariables[3] == (DM_FreeVariables)[3]);
		TestTrue("Check if fourth element has same name.",
		         DataModelTestVariables::FreeVariables[3].GetName() == (DM_FreeVariables)[3].GetName());
		TestTrue("Check if fifth element is equal.", DataModelTestVariables::FreeVariables[4] == (DM_FreeVariables)[4]);
		TestTrue("Check if fifth element has same name.",
		         DataModelTestVariables::FreeVariables[4].GetName() == (DM_FreeVariables)[4].GetName());
	}
	catch (...)
	{
		TestTrue("GetFreeVariables Start Content consistency Failed.", false);
	}

	// Deconstruct Everything

	// Return true to finish your test
	return true;
}
