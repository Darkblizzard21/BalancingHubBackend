// Copyright Pirmin Pfeifer. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BalanceableFloatBundle.h"
#include "BalanceableFloatComponent.generated.h"

/**
* Struct that imitates a read only float
* -Carries a internal float
* -Can be balanced in the editor window
* -Functions like a real float implementing all operations a normal float can
*/
USTRUCT()
struct BALANCEABLEVARIABLES_API FBalanceableFloat
{
	GENERATED_BODY()

	friend class UBalanceableFloatComponent;
public:
	/**
	* Used in the BALANCEABLEFLOAT Macro
	* @attention Should not be used! use the BALANCEABLEFLOAT Macro instead
	* @returns the new empty initialized FMIDIFloat
	* @see UBalanceableVariablesLibrary
	*/
	FBalanceableFloat() = default;
	/**
	* Used to get a NewFMIDIFloat in the BALANCEABLEFLOAT & INITIALIZE_BALANCEABLEFLOAT MACROs
	* @attention do not call for balancableFloatDeclartion
	* @param StartValue - initial value of the internal float
	* @returns the new fully initialized FMIDIFloat
	* @see UBalanceableVariablesLibrary
	*/
	static FBalanceableFloat NewFBalanceableFloat(const float StartValue);

	/**
	* Easy way to get a copy of the internal value
	* @brief getter for the internal value
	* @returns the internal float
	*/
	float GetValue() const { return Value; };

	/**
	* By passes the float imitation and gets pointer to the Struct and not the internal float
	* @returns pointer to this struct
	*/
	FBalanceableFloat* GetStructPointer() { return this; };

	
private:
	float Value = 0;

public:
	//Begin: Float "Interface"
	//Constant Casting
	operator float() const { return Value; };
	
	//Pointer Operators
	const float* operator&() { return &Value; }
	const float* operator&() const { return &Value; }

	//Math Operators
	float operator+(const float Other) const { return Value + Other; }

	float operator-(const float Other) const { return Value - Other; }

	float operator*(const float Other) const { return Value * Other; }
	FVector operator*(const FVector Other) const { return Value * Other; }
	FVector2D operator*(const FVector2D Other) const { return Value * Other; }
	FVector4 operator*(const FVector4 Other) const { return Value * Other; }

	float operator/(const float Other) const { return Value / Other; }

	//Increment&Decrement

	//Boolean Operators
	bool operator==(const float Other) const { return Value == Other; }

	bool operator!=(const float Other) const { return Value != Other; }

	bool operator>(const float Other) const { return Value > Other; }

	bool operator>=(const float Other) const { return Value >= Other; }

	bool operator<(const float Other) const { return Value < Other; }

	bool operator<=(const float Other) const { return Value <= Other; }
	//End: Float "Interface"
};

/**
 * This is a component declared by the BALANCEABLEFLOAT Macro and initiated by the INITIALIZE_BALANCEABLEFLOAT Macro
 * -do not declare or initialize yourself
 */
UCLASS()
class BALANCEABLEVARIABLES_API UBalanceableFloatComponent : public UActorComponent, public IBalanceableFloatAccess
{
	GENERATED_BODY()

public:
	/*
	 * @attention do not call for balancableFloatDeclartion
	 */
	UBalanceableFloatComponent();
	virtual void DestroyComponent(bool bPromoteChildren = false) override;
	virtual void Activate(bool bReset) override;
	virtual void Deactivate() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayR) override;

	//Begin: IBalanceableFloatAccess Interface
	/**
	* Called by the BalanceableFloatBundle with the same identifier -never call this function by your self
	* @param NewValue new value for the float
	* @see UBalanceableVariablesLibrary
	*/
	virtual void UpdateValue(float NewValue) override;
	/**
	* Easy way to access the Identifier of this component
	* @returns the Identifier
	*/
	virtual FString GetIdentifier() const override { return Identifier; }
	/**
	* Easy way to access the Name of this component
	* @returns the Name
	*/
	virtual FString GetVariableName() const override { return  Name;}
	//End: IBalanceableFloatAccess Interface

	/**
	* Easy way to access the FBalanceableFloat of this component
	* @returns the FBalanceableFloat
	*/
	FBalanceableFloat* GetBalanceableFloat() const { return BalanceableFloat; };

	/**
	* Called at the end of the Initialization in the INITIALIZE_BALANCEABLEFLOAT Macro  -never call this function by your self
	* @param Identifier - Identifier of the Variable for the System
	* @param Name - Name that is shown in the frontend
	* @param BalanceableFloat - Reference to the managed BalanceableFloat
	* 
	* @return false when the initialization is already finished
	*/
	bool FinishInitialization(FString Identifier, FString Name, FBalanceableFloat* BalanceableFloat);

protected:
	FString Identifier;
	FString Name;
	FBalanceableFloat* BalanceableFloat;

	bool bIsInPlay = false;
};
