// Copyright Pirmin Pfeifer. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BalanceableFloatComponent.h"
#include "BalanceableVariablesLibrary.h"
#include "BalanceableVariablesTestingActor.generated.h"
/**
* This ABFCTestingActor (ABalanceableFlaotComponentTestingActor) is used in the Test for the component.
*/
UCLASS()
class ABalanceableVariablesTestingActor : public AActor
{
	GENERATED_BODY()

public:

	BALANCEABLEFLOAT(BalanceableMacroFloat1)
	BALANCEABLEFLOAT(BalanceableMacroFloat2)
	BALANCEABLEFLOAT(BalanceableMacroFloat3)
private:
	bool bInit = false;
public:
	UPROPERTY(Category = BalanceableVariables, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBalanceableFloatComponent* BalanceableFloatComponent;


	ABalanceableVariablesTestingActor() 
	{ 
		PrimaryActorTick.bCanEverTick = false;
		BalanceableFloatComponent = CreateDefaultSubobject<UBalanceableFloatComponent>("BalanceableFloatComponent");
		const float DefaultValue = 1.f;
		INITIALIZE_BALANCEABLEFLOAT(BalanceableMacroFloat1, DefaultValue);
		INITIALIZE_BALANCEABLEFLOAT(BalanceableMacroFloat2, DefaultValue);
		INITIALIZE_BALANCEABLEFLOAT(BalanceableMacroFloat3, DefaultValue);
	};
};
