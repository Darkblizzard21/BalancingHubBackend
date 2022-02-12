// Copyright Pirmin Pfeifer & Quirin Maier. All Rights Reserved.


#include "BalanceableFloatBundle.h"
#include "BalanceableVariablesController.h"

FBalanceableFloatBundle::FBalanceableFloatBundle(): CurrentValue(0)
{
}

bool IBalanceableFloatAccess::RegisterSelf(const float DefaultValue)
{
	return UBalanceableVariablesController::RegisterBalanceableFloat(this, DefaultValue);
}

void FBalanceableFloatBundle::UpdateVariable(const float NewValue)
{
	CurrentValue = NewValue;
	RegisteredBalanceableFloats.Remove(nullptr);
	for (int32 i = 0; i < RegisteredBalanceableFloats.Num(); i++)
	{
		auto x = RegisteredBalanceableFloats[i];
		x->UpdateValue(NewValue);
	}
}

FBalanceableFloatBundle* FBalanceableFloatBundle::NewBalanceableFloatBundle(
	const FString ChosenIdentifier, const FString ChosenName, const float DefaultValue)
{
	FBalanceableFloatBundle* NewBalanceableFloatBundle = new FBalanceableFloatBundle();

	NewBalanceableFloatBundle->Identifier = ChosenIdentifier;
	NewBalanceableFloatBundle->Name = ChosenName;
	NewBalanceableFloatBundle->RegisteredBalanceableFloats = TArray<IBalanceableFloatAccess*>();

	NewBalanceableFloatBundle->CurrentValue = DefaultValue;
	//Try to get Value from Data //Data Still Due To Implement

	return NewBalanceableFloatBundle;
}

bool FBalanceableFloatBundle::AddBalanceableFloat(IBalanceableFloatAccess* BalanceableFloat)
{
	if (!BalanceableFloat->GetIdentifier().Equals(Identifier, ESearchCase::IgnoreCase)) return false;
	if (RegisteredBalanceableFloats.Contains(BalanceableFloat)) return false;

	BalanceableFloat->UpdateValue(CurrentValue);
	RegisteredBalanceableFloats.Add(BalanceableFloat);

	return true;
}

bool FBalanceableFloatBundle::RemoveBalanceableFloat(IBalanceableFloatAccess* BalanceableFloat)
{
	if (!BalanceableFloat->GetIdentifier().Equals(Identifier, ESearchCase::IgnoreCase)) return false;
	if (!RegisteredBalanceableFloats.Contains(BalanceableFloat)) return false;

	RegisteredBalanceableFloats.Remove(BalanceableFloat);

	return true;
}
