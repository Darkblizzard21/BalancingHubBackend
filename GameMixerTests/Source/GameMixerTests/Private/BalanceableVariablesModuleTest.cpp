// Copyright Pirmin Pfeifer. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "TestUtility.h"
#include "BalanceableVariablesController.h"
#include "BalanceableVariablesLibrary.h"
#include "TestDoubles/BalanceableVariablesTestingActor.h"

class FRuntimeTestUpdateOrder : public IAutomationLatentCommand
{
public:
	FRuntimeTestUpdateOrder(FAutomationTestBase* Test, UWorld* World, ABalanceableVariablesTestingActor* Actor) : Test(Test), World(World), Actor(Actor)
	{
		i = 0;
		Identifier = "";
		LastValue = 0;
		UpdateValue = 0;
	}
	virtual ~FRuntimeTestUpdateOrder() override
	{
		//Clean
		World->DestroyActor(Actor);
		World->CleanupActors();
		World->DestroyWorld(false,World);
		Actor = nullptr;
	}
	virtual bool Update() override;
private:
	FAutomationTestBase* Test;
	UWorld* World;
	ABalanceableVariablesTestingActor* Actor;
	int i;
	FString Identifier;
	float LastValue;
	float UpdateValue;
};

bool FRuntimeTestUpdateOrder::Update()
{
	switch (i)
	{
	case 5:
		//Make Update Request && check that the change dose not happen immedatly
		Identifier = Actor->BalanceableMacroFloat1Component->GetIdentifier();
		LastValue = Actor->BalanceableMacroFloat1;
		UpdateValue = 42.f;
		if (UpdateValue == LastValue) UpdateValue++;
		UBalanceableVariablesController::IssueFloatUpdate(Identifier, UpdateValue);
		Test->TestFalse("Test that IssueFloatUpdate dose not update the float immediatly", UpdateValue == Actor->BalanceableMacroFloat1);
		Test->TestTrue("Test Consistency of BalanceableFloat on update", LastValue == Actor->BalanceableMacroFloat1);
		break;

	case 6:
		//Check if it is updated in the next frame
		Test->TestTrue("Test if the update is done after one frame", UpdateValue == Actor->BalanceableMacroFloat1);
		return true;
	default:
		break;
	}
	i++;
	return false;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBalanceableVariablesTestUpdateOrder, "GameMixerTests.BalanceableVariables.FrameConsitencyTest",
	EAutomationTestFlags::EditorContext |
	EAutomationTestFlags::ProductFilter)
	bool FBalanceableVariablesTestUpdateOrder::RunTest(const FString& Parameters) {
	//Initialize Variables
	//Initialization
	AutomationOpenMap(TEXT("/GameMixerTests/Maps/T_TestWorld.t_TestWorld"));
	UWorld* World = TestUtility::GetTestWorld();

	//Create Actors
	ABalanceableVariablesTestingActor* Actor = World->SpawnActor<ABalanceableVariablesTestingActor>();

	//Tests
	//ADD_LATENT_AUTOMATION_COMMAND(FRuntimeTestUpdateOrder(this, World, Actor));
	FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShareable(new FRuntimeTestUpdateOrder(this, World, Actor)));
	// Return true to finish your test
	return true;
}
