// Copyright Pirmin Pfeifer & Quirin Maier. All Rights Reserved.


#include "BalanceableVariablesController.h"

#include "Containers/Queue.h"
#include "UpdateRequest.h"
//Static Inits
bool UBalanceableVariablesController::bIsInitialized = false;
int32 UBalanceableVariablesController::FrameCounterInternal = 0;
TWeakPtr<FNewVariablesPushReceiver> UBalanceableVariablesController::Receiver = nullptr;
//Float Variables
TQueue<TUpdateRequest<float>, EQueueMode::Mpsc>* FloatUpdateQueue = nullptr;
TMap<FString, FBalanceableFloatBundle*>* BlanceableFloatBundles = nullptr;


void UBalanceableVariablesController::StartupMIDIVariablesController()
{
	if (ensure(!bIsInitialized))
	{
#if WITH_EDITOR
		//Initialize BlanceableFloat Variables
		FloatUpdateQueue = new TQueue<TUpdateRequest<float>, EQueueMode::Mpsc>();
		if (BlanceableFloatBundles == nullptr)
		{
			BlanceableFloatBundles = new TMap<FString, FBalanceableFloatBundle*>;
		}

		//Set Defaults
		FrameCounterInternal = 0;
#endif
		//Mark as Initialized
		bIsInitialized = true;
	}
}

void UBalanceableVariablesController::ShutdownMIDIVariablesController()
{
	if (bIsInitialized)
	{
		bIsInitialized = false;

		//Clear Queue to save resources
		FloatUpdateQueue->~TQueue();
	}
}

bool UBalanceableVariablesController::IssueFloatUpdate(const FString Identifier, const float NewValue)
{
	if (bIsInitialized)
	{
#if WITH_EDITOR
		if (BlanceableFloatBundles->Contains(Identifier))
		{
			const TUpdateRequest<float> NewRequest = TUpdateRequest<float>::NewTUpdateRequest(Identifier, NewValue);
			FloatUpdateQueue->Enqueue(NewRequest);
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ERROR: Identifier \"%s\" is not vaild!"), *Identifier);
		}
#endif
	}
	return false;
}

bool UBalanceableVariablesController::TryGetCurrentValue(const FString Identifier, float& OutValue)
{
	OutValue = 0;
	if (bIsInitialized)
	{
		FBalanceableFloatBundle** Bundle = BlanceableFloatBundles->Find(Identifier);
		if (Bundle != nullptr && (*Bundle) != nullptr)
			OutValue = (**Bundle).GetCurrentValue();
		return true;
	}
	return false;
}

TArray<FBalanceableVariableInfo>* UBalanceableVariablesController::GetAllCurrentFloatIdentifier()
{
	TArray<FBalanceableFloatBundle*> Values;
	BlanceableFloatBundles->GenerateValueArray(Values);
	TArray<FBalanceableVariableInfo>* Result = new TArray<FBalanceableVariableInfo>;
	for (auto Value : Values)
	{
		auto Next = FBalanceableVariableInfo();
		Next.Identifier = Value->GetIdentifier();
		Next.Name = Value->GetVariableName();
		Next.Type = Float;
		Result->Add(Next);
	}
	return Result;
}


void UBalanceableVariablesController::InjectOrOverrideBalanceableFloat(const FString Name, const FString Id,
                                                                       const float Value)
{
	if (bIsInitialized)
	{
		const bool bBundleExists = BlanceableFloatBundles->Contains(Id);
		if (bBundleExists)
		{
			FBalanceableFloatBundle* BalanceableFloatBundle = *BlanceableFloatBundles->Find(Id);
			BalanceableFloatBundle->UpdateVariable(Value);
		}
		else
		{
			FBalanceableFloatBundle* BalanceableFloatBundle = FBalanceableFloatBundle::NewBalanceableFloatBundle(
				Id,
				Name,
				Value);
			BlanceableFloatBundles->Add(Id, BalanceableFloatBundle);
		}
	}
}

bool UBalanceableVariablesController::RegisterBalanceableFloat(IBalanceableFloatAccess* BalanceableFloat,
                                                               float DefaultValue)
{
	if (bIsInitialized)
	{
		const FString RegistrationIdentifier = BalanceableFloat->GetIdentifier();
		const bool bBundleExists = BlanceableFloatBundles->Contains(RegistrationIdentifier);
		if (bBundleExists)
		{
			FBalanceableFloatBundle* BalanceableFloatBundle = *BlanceableFloatBundles->Find(RegistrationIdentifier);
			BalanceableFloatBundle->AddBalanceableFloat(BalanceableFloat);
		}
		else
		{
			FBalanceableFloatBundle* BalanceableFloatBundle = FBalanceableFloatBundle::NewBalanceableFloatBundle(
				RegistrationIdentifier,
				BalanceableFloat->GetVariableName(),
				DefaultValue);
			BlanceableFloatBundles->Add(RegistrationIdentifier, BalanceableFloatBundle);
			BalanceableFloatBundle->AddBalanceableFloat(BalanceableFloat);

			//Push new if bound
			if (Receiver.IsValid())
			{
				FBalanceableVariableInfo NewInfo = FBalanceableVariableInfo();
				NewInfo.Identifier = BalanceableFloatBundle->GetIdentifier();
				NewInfo.Name = BalanceableFloatBundle->GetVariableName();
				NewInfo.Type = Float;
				Receiver.Pin().Get()->HandleNewData(NewInfo);
			}
		}
		return true;
	}
	return false;
}

bool UBalanceableVariablesController::UnRegisterBalanceableFloat(IBalanceableFloatAccess* BalanceableFloat)
{
	if (bIsInitialized)
	{
		const FString RegistrationIdentifier = BalanceableFloat->GetIdentifier();
		const bool bBundleExists = BlanceableFloatBundles->Contains(RegistrationIdentifier);
		if (bBundleExists)
		{
			FBalanceableFloatBundle* BalanceableFloatBundle = *BlanceableFloatBundles->Find(RegistrationIdentifier);
			return BalanceableFloatBundle->RemoveBalanceableFloat(BalanceableFloat);
		}
	}
	return false;
}

void UBalanceableVariablesController::TickApplyChanges()
{
	if (bIsInitialized
	)
	{
		FrameCounterInternal++;
		TUpdateRequest<float> NextFloatUpdate;
		while (!FloatUpdateQueue->IsEmpty())
		{
			FloatUpdateQueue->Dequeue(NextFloatUpdate);
			if (BlanceableFloatBundles->Contains(NextFloatUpdate.GetIdentifier()))
			{
				FBalanceableFloatBundle* CurrentBundle = *BlanceableFloatBundles->Find(NextFloatUpdate.GetIdentifier());
				CurrentBundle->UpdateVariable(NextFloatUpdate.GetValue());
			}
		}
	}
}
