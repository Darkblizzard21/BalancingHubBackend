// Copyright Pirmin Pfeifer & Quirin Maier. All Rights Reserved.


#include "BalanceableFloatBP.h"

#include <string>

#include "BalanceableVariablesController.h"
#include "BalanceableVariablesLibrary.h"
#include "GameFramework/Actor.h"

UBalanceableFloatBP* UBalanceableFloatBP::Initialize(const FString IdentifierIn, const float DefaultValue)
{
	if (FMath::IsNearlyEqual(Value, INFINITY))
	{
		Identifier = FLOAT_IDENTIFIER_FROM_STRING(IdentifierIn, GetOuter());
		Name = VARIABLENAME_FROM_STRING_AND_CLASS(IdentifierIn, GetOuter());
		RegisterSelf(DefaultValue);
	}
	check(!FMath::IsNearlyEqual(Value, INFINITY));
	return this;
}

UBalanceableFloatBP* UBalanceableFloatBP::InitializeWithOwnName(float DefaultValue)
{
	if (FMath::IsNearlyEqual(Value, INFINITY))
	{
		const FString NameIdentifier = GetName();
		Identifier = FLOAT_IDENTIFIER_FROM_STRING(NameIdentifier, this);
		Name = VARIABLENAME_FROM_STRING_AND_CLASS(NameIdentifier, this);
		RegisterSelf(DefaultValue);
	}
	check(!FMath::IsNearlyEqual(Value, INFINITY));
	return this;
}

void UBalanceableFloatBP::DestroyComponent(const bool bPromoteChildren)
{
	UBalanceableVariablesController::UnRegisterBalanceableFloat(this);
	Super::DestroyComponent(bPromoteChildren);
}

void UBalanceableFloatBP::Activate(const bool bReset)
{
	RegisterSelf(Value);
	Super::Activate(bReset);
}

void UBalanceableFloatBP::Deactivate()
{
	UBalanceableVariablesController::UnRegisterBalanceableFloat(this);
	Super::Deactivate();
}

void UBalanceableFloatBP::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UBalanceableVariablesController::UnRegisterBalanceableFloat(this);
	Super::EndPlay(EndPlayReason);
}


float UBalanceableFloatBP::GetValue() const
{
	UE_LOG(LogTemp, Warning, TEXT("Text,  %f "), Value);
	return Value;
}

FString UBalanceableFloatBP::GetIdentifier() const
{
	return Identifier;
}

FString UBalanceableFloatBP::GetVariableName() const
{
	return Name;
}

void UBalanceableFloatBP::UpdateValue(const float NewValue)
{
	Value = NewValue;
	UE_LOG(LogTemp, Warning, TEXT("Text,  %f "), Value);
	check(!FMath::IsNearlyEqual(Value, INFINITY));
}
