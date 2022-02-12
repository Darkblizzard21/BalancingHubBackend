// Copyright Pirmin Pfeifer & Quirin Maier. All Rights Reserved.

#pragma once

//UE4 Classes
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
//BalanceableClasses:
#include "BalanceableFloatBundle.h"
//Generated:
#include "BalanceableVariablesController.generated.h"

// Forwarded Declarations:
struct FBalanceableVariableInfo;
enum EBalanceableVariableType;

class FNewVariablesPushReceiver;

/**
 * Controllers all Backend Variables 
 */
UCLASS()
class UBalanceableVariablesController : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Called from FMIDIVariablesModule to startup the VariablesController.  Don't call this yourself. */
	static void StartupMIDIVariablesController();
	/** Called from FMIDIVariablesModule to shutdown the VariablesController.  Don't call this yourself. */
	static void ShutdownMIDIVariablesController();

	//Begin: BalanceableVariablesInterface
	/**
	* @brief Used to change a BalanceableFloat by identifier
	* @param Identifier - of the float that should be changed
	* @param NewValue - updated Value
	* @return true if the identifier existed
	*/
	static BALANCEABLEVARIABLES_API bool IssueFloatUpdate(FString Identifier, float NewValue);


	/**
	 * @brief Fetches the current value of an identifier if it is present in the data
	 * @param Identifier - of the float that should be read
	 * @param OutValue - value if its present or zero when nothing is present
	 * @return ture if the Identifier was present in the data 
	 */
	static BALANCEABLEVARIABLES_API bool TryGetCurrentValue(const FString Identifier, float& OutValue);

	/**
	* @brief Fetches all present FBalanceableVariableInfo 
	* @return All present FBalanceableVariableInfo
	*/
	static BALANCEABLEVARIABLES_API TArray<FBalanceableVariableInfo>* GetAllCurrentFloatIdentifier();

	/** Binds a FNewVariablesPushReceiver, overriding the current receiver if on is present
	* !!!should only be called in the data model module!!!
	* @param NewReceiver data that should receive the update about new variables
	*/
	static BALANCEABLEVARIABLES_API void BindReceiver(const TWeakPtr<FNewVariablesPushReceiver> NewReceiver)
	{
		Receiver = NewReceiver;
	};

	/** Injects a new BalancableFloatBundle. Is used during deserialization of the datamodel and should NOT be called afterwards
	* @param Name Display name of the float
	* @param Id Identifier of the float
	* @param Value Current Value of the float
	*/
	static BALANCEABLEVARIABLES_API void InjectOrOverrideBalanceableFloat(FString Name, FString Id, float Value);
	//End: BalanceableVariablesInterface


	//Begin: VariablesRegistration
	/**
	* Called by Variable that Implements the IBalanceableFloatAccess Interface
	* @param BalanceableFloat - reference to the IBalanceableFloatAccess that wants to be registered
	* @param DefaultValue - the value that should be used in case of it being the fist variable with it is identifier and the data is not available
	*/
	static bool RegisterBalanceableFloat(IBalanceableFloatAccess* BalanceableFloat, float DefaultValue = 1.f);
	/**
	* Called by Variable that Implements the IBalanceableFloatAccess Interface on its own destruction
	* @param BalanceableFloat - reference to the IBalanceableFloatAccess that wants to be removed from their register
	*/
	static bool UnRegisterBalanceableFloat(IBalanceableFloatAccess* BalanceableFloat);
	//End: VariablesRegistration

	/** Called every frame by FMIDIVariablesModule::Tick()) to update all Variables in the UpdateQueue.  Don't call this yourself. */
	static void TickApplyChanges();

private:
	static bool bIsInitialized;
	static int32 FrameCounterInternal;
	static TWeakPtr<FNewVariablesPushReceiver> Receiver;
};

struct BALANCEABLEVARIABLES_API FBalanceableVariableInfo
{
	FString Identifier;
	FString Name;
	EBalanceableVariableType Type;
};

enum BALANCEABLEVARIABLES_API EBalanceableVariableType
{
	Float
};

/*
 * Class that get implemented by the datamodel
 * Used to push new variables into the datamodel
*/
class BALANCEABLEVARIABLES_API FNewVariablesPushReceiver
{
public:
	virtual ~FNewVariablesPushReceiver() = default;
	
	/* Called by the BalanceableVariablesController when it receives new variables
	 * @param Data Id,Name and current value of the new balancableFloat 
	 */
	virtual void HandleNewData(FBalanceableVariableInfo Data) = 0;
};
