// Copyright Pirmin Pfeifer. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "TestUtility.h"
#include "TestDoubles/BalanceableVariablesTestingActor.h"
#include "BalanceableFloatComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBalanceableFloatComponentTestFininishInit,
                                 "GameMixerTests.BalanceableVariables.BalanceableFloatComponent.FinishInitialization",
                                 EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::ProductFilter)

bool FBalanceableFloatComponentTestFininishInit::RunTest(const FString& Parameters)
{
	//Initialization
	AutomationOpenMap(TEXT("/GameMixerTests/Maps/T_TestWorld.t_TestWorld"));
	UWorld* World = TestUtility::GetTestWorld();
	ABalanceableVariablesTestingActor* Actor = World->SpawnActor<ABalanceableVariablesTestingActor>();
	UBalanceableFloatComponent* BFComponent = Actor->BalanceableFloatComponent;
	FBalanceableFloat BalanceableFloat = FBalanceableFloat::NewFBalanceableFloat(0.f);

	//Tests
	bool FunctionResult = BFComponent->FinishInitialization("Test", "Test", BalanceableFloat.GetStructPointer());
	TestTrue("Check if FinishInitialization returns true on its first call", FunctionResult);

	FunctionResult = BFComponent->FinishInitialization("Test", "Test", BalanceableFloat.GetStructPointer());
	FunctionResult = BFComponent->FinishInitialization("Test", "Test", BalanceableFloat.GetStructPointer()) ||
		FunctionResult;
	FunctionResult = BFComponent->FinishInitialization("Test", "Test", BalanceableFloat.GetStructPointer()) ||
		FunctionResult;
	TestFalse("Check if FinishInitialization returns false on every call after the first", FunctionResult);
	//CleanUp
	World->DestroyActor(Actor);

	// Return true to finish your test
	return true;
}


class FRuntimePersistenceTest : public IAutomationLatentCommand
{
public: // Persistence
	FRuntimePersistenceTest(FAutomationTestBase* test, UWorld* World) : test(test), World(World)
	{
		i = 0;
		Actor = World->SpawnActor<ABalanceableVariablesTestingActor>();
		UBalanceableFloatComponent* BFComponent = Actor->BalanceableFloatComponent;
		BalanceableFloat = FBalanceableFloat::NewFBalanceableFloat(0.f);
		BFComponent->FinishInitialization("Test", "Test", BalanceableFloat.GetStructPointer());
	}

	virtual ~FRuntimePersistenceTest()
	{
		World->DestroyActor(Actor);
		Actor = nullptr;
		i = 1;
	}

	virtual bool Update() override;
private:
	FAutomationTestBase* test;
	UWorld* World;
	FBalanceableFloat BalanceableFloat;
	ABalanceableVariablesTestingActor* Actor;
	int i;
};

bool FRuntimePersistenceTest::Update()
{
	if (i >= 8)
		return true;
	if (Actor)
	{
		//Test if Update Still works
		UBalanceableFloatComponent* BFComponent = Actor->BalanceableFloatComponent;
		const float NewValue = BFComponent->GetBalanceableFloat()->GetValue() + 10.f;
		Actor->BalanceableFloatComponent->UpdateValue(NewValue);
		test->TestEqual("Test for persistence", NewValue, BFComponent->GetBalanceableFloat()->GetValue());
	}

	i++;
	//Tries next tick to pass the test
	return false;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBalanceableFloatComponentTestIBalanceableFloatAccessCG,
                                 "GameMixerTests.BalanceableVariables.BalanceableFloatComponent.IBalanceableFloatAccessGarbageCollectionInteraction",
                                 EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::ProductFilter)

bool FBalanceableFloatComponentTestIBalanceableFloatAccessCG::RunTest(const FString& Parameters)
{
	//Initialization
	AutomationOpenMap(TEXT("/GameMixerTests/Maps/T_TestWorld.t_TestWorld"));
	UWorld* World = TestUtility::GetTestWorld();

	//Test that the system isn't cleared out by GC
	ADD_LATENT_AUTOMATION_COMMAND(FRuntimePersistenceTest(this, World));


	// Return true to finish your test
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBalanceableFloatComponentTestIBalanceableFloatAccess,
                                 "GameMixerTests.BalanceableVariables.BalanceableFloatComponent.IBalanceableFloatAccess",
                                 EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::ProductFilter)

bool FBalanceableFloatComponentTestIBalanceableFloatAccess::RunTest(const FString& Parameters)
{
	//Initialization
	AutomationOpenMap(TEXT("/GameMixerTests/Maps/T_TestWorld.t_TestWorld"));
	UWorld* World = TestUtility::GetTestWorld();
	ABalanceableVariablesTestingActor* Actor = World->SpawnActor<ABalanceableVariablesTestingActor>();
	UBalanceableFloatComponent* BFComponent = Actor->BalanceableFloatComponent;
	FBalanceableFloat BalanceableFloat = FBalanceableFloat::NewFBalanceableFloat(0.f);
	BFComponent->FinishInitialization("Test", "Test", BalanceableFloat.GetStructPointer());

	//Test UpdateValue
	float NewValue = 10.f;
	BFComponent->UpdateValue(NewValue);
	TestEqual("Check if UpdateValue works with positive float", NewValue, BalanceableFloat.GetValue());
	NewValue = -0.352;
	BFComponent->UpdateValue(NewValue);
	TestEqual("Check if UpdateValue works with negative float", NewValue, BalanceableFloat.GetValue());
	NewValue = INFINITY;
	BFComponent->UpdateValue(NewValue);
	TestEqual("Check if UpdateValue works with infinty", NewValue, BalanceableFloat.GetValue());
	NewValue = 0;
	BFComponent->UpdateValue(NewValue);
	TestEqual("Check if UpdateValue works with zero", NewValue, BalanceableFloat.GetValue());

	//Test Get Identifier
	TestTrue("Check if GetIdentifier is working", BFComponent->GetIdentifier().Equals("Test"));

	//Test that the system isnt cleared out by GC
	ADD_LATENT_AUTOMATION_COMMAND(FRuntimePersistenceTest(this, World));


	//CleanUp
	World->DestroyActor(Actor);
	// Return true to finish your test
	return true;
}
