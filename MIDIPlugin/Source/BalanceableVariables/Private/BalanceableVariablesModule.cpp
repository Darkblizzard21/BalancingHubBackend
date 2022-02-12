// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Tickable.h"
#include "BalanceableVariablesController.h"

class FBalanceableVariablesModule : public IModuleInterface, public FTickableGameObject
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// FTickableGameObject interface
	virtual void Tick(float DeltaTime) override;

	virtual ETickableTickType GetTickableTickType() const override
	{
		return ETickableTickType::Always;
	}
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UMIDIVariablesController, STATGROUP_Tickables);
	}
	virtual bool IsTickableWhenPaused() const override
	{
		return true;
	}
	virtual bool IsTickableInEditor() const override
	{
		return true;
	}

private:

	/** The last frame number we were ticked.  We don't want to tick multiple times per frame */
	uint32 LastFrameNumberWeTicked = 0;
};

IMPLEMENT_MODULE(FBalanceableVariablesModule, BalanceableVariables)

#define LOCTEXT_NAMESPACE "FBalanceableVariablesModule"

void FBalanceableVariablesModule::StartupModule()
{
#if WITH_EDITOR
	this->LastFrameNumberWeTicked = INDEX_NONE;
#endif
	UBalanceableVariablesController::StartupMIDIVariablesController();

}

void FBalanceableVariablesModule::ShutdownModule()
{
	UBalanceableVariablesController::ShutdownMIDIVariablesController();
}

void FBalanceableVariablesModule::Tick(float DeltaTime)
{
#if WITH_EDITOR
	if (LastFrameNumberWeTicked != GFrameCounter)
	{
		// Update the MIDI Variables Controller
		UBalanceableVariablesController::TickApplyChanges();

		LastFrameNumberWeTicked = GFrameCounter;
	}
#endif
}

#undef LOCTEXT_NAMESPACE