// Copyright Pirmin Pfeifer. All Rights Reserved.


#include "BalanceableFloatComponent.h"
#include "BalanceableVariablesController.h"
#include "Math/UnrealMathUtility.h"

FBalanceableFloat FBalanceableFloat::NewFBalanceableFloat(const float StartValue)
{
	FBalanceableFloat BalanceableFloat = FBalanceableFloat();
	BalanceableFloat.Value = StartValue;
	return BalanceableFloat;
}


void UBalanceableFloatComponent::DestroyComponent(const bool bPromoteChildren)
{
	UBalanceableVariablesController::UnRegisterBalanceableFloat(this);
	Super::DestroyComponent(bPromoteChildren);
}

void UBalanceableFloatComponent::Activate(const bool bReset)
{
	RegisterSelf(BalanceableFloat->GetValue());
	Super::Activate(bReset);
}

void UBalanceableFloatComponent::Deactivate()
{
	UBalanceableVariablesController::UnRegisterBalanceableFloat(this);
	Super::Deactivate();
}

void UBalanceableFloatComponent::BeginPlay()
{
	if(BalanceableFloat != nullptr)
		RegisterSelf(BalanceableFloat->GetValue());
	bIsInPlay = true;
	Super::BeginPlay();
}

void UBalanceableFloatComponent::EndPlay(const EEndPlayReason::Type EndPlayR)
{
	UBalanceableVariablesController::UnRegisterBalanceableFloat(this);
	bIsInPlay = false;
	Super::EndPlay(EndPlayR);
}

void UBalanceableFloatComponent::UpdateValue(const float NewValue)
{
	BalanceableFloat->Value = NewValue;
}

UBalanceableFloatComponent::UBalanceableFloatComponent()
{
	//Initialize Variables
	BalanceableFloat = nullptr;
	Identifier = "";
}

bool UBalanceableFloatComponent::FinishInitialization(const FString ChosenIdentifier, const FString ChosenName, FBalanceableFloat* ChosenBalanceableFloat)
{
	if (BalanceableFloat == nullptr && Identifier.IsEmpty())
	{
		Identifier = ChosenIdentifier;
		Name = ChosenName;
		BalanceableFloat = ChosenBalanceableFloat;

		if(bIsInPlay)
			RegisterSelf(BalanceableFloat->GetValue());

		return true;
	}
	return false;
}