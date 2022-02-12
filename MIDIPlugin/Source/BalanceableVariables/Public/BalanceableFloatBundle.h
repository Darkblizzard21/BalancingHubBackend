// Copyright Pirmin Pfeifer & Quirin Maier. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/Interface.h" 
#include "BalanceableFloatBundle.generated.h"

/**
 *
 */
UINTERFACE()
class UBalanceableFloatAccess : public UInterface
{
	GENERATED_BODY()
};

/*
 * Interface to access a balancable float.
 * Used by the BalancableVariablesController to update the balancable floats
 */
class IBalanceableFloatAccess
{
	GENERATED_BODY()

public:
	
	virtual void UpdateValue(float NewValue) = 0;
	virtual FString GetIdentifier() const = 0;
	virtual FString GetVariableName() const = 0;
protected:
	/**
	* Registers the Variables that implements this to the Controller
	* @param DefaultValue - the value that should be used in case of it being the first variable with its identifier and the data is not available
	* @return true when the variable is registered
	*/
	bool RegisterSelf(float DefaultValue);
};

/**
 * A Bundle that is used to bundle up all BalanceableFloat with the same identifier
 */
class FBalanceableFloatBundle
{

private:
	FBalanceableFloatBundle();

public:
	/**
	* Constructor that should be used to creat this Object
	* @param Identifier - of the float that should be controlled with the bundle
	* @param DefaultValue - the default start value
	*/
	static FBalanceableFloatBundle* NewBalanceableFloatBundle(FString Identifier, FString Name, float DefaultValue);
	/**
	* Called by Controller to register new BalanceableFloats
	* @param BalanceableFloat - Pointer to the BlanceableFloat
	* @return true when the BalanceableFloat was added
	*/
	bool AddBalanceableFloat(IBalanceableFloatAccess* BalanceableFloat);
	/**
	* Called by Controller to remove BalanceableFloats on their destruction
	* @param BalanceableFloat - Pointer to the BlanceableFloat
	* @return true when the BalanceableFloat existed 
	*/
	bool RemoveBalanceableFloat(IBalanceableFloatAccess* BalanceableFloat);

	/**
	* Called by Controller in TickApplyChanges when there is a new value
	* @param NewValue - new value all float in this bundle
	*/
	void UpdateVariable(float NewValue);
	/** Grants Access to the Identifier for checking*/
	FString GetIdentifier() const { return Identifier; };

	FString GetVariableName() const { return  Name; }
	/** Grants Access to the CurrentValue*/
	float GetCurrentValue() const { return CurrentValue; };
private: 
	FString Identifier;
	FString Name;
	float CurrentValue;
	TArray<IBalanceableFloatAccess*> RegisteredBalanceableFloats;
};
