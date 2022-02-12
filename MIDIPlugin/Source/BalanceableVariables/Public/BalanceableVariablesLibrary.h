// Copyright Pirmin Pfeifer. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
//MIDIVariables:
#include "BalanceableFloatComponent.h"
#include "BalanceableFloatBP.h"
//Forward Required Imports
#include <typeinfo>


#include "BalanceableVariablesLibrary.generated.h"
/** C++ API: BalanceableVariablesLibrary
 *	====================================
 *	This library contains all macros that are needed to declare your BalancableFloats.
 *
 *	Usage Example
 * 	-------------
 * 	Declare publicly in your header with a _Name_:
 * 	
 * 		BALANCEABLEFLOAT(_Name_)
 *
 *	You also need to initialize your balancable float in the constructor of the class with
 *	
 *		float initialValue = 10.f
 *		INITIALIZE_BALANCEABLEFLOAT(_Name_, initialValue)
 *
 *	Afterwards you can use your blancable float as readonly float in your class.
 *	But keep in mind that any variable you assign the balanacable float to will not get automatically updated.
 */
UCLASS()
class BALANCEABLEVARIABLES_API UBalanceableVariablesLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	* Declares a FMIDIFloat in an Actors Header
	* @param BalanceableFloatName - the name that should be used for the float
	*/
	#define BALANCEABLEFLOAT(BalanceableFloatName) UPROPERTY(Category = BalanceableVariables, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))\
	class UBalanceableFloatComponent* BalanceableFloatName##Component;\
	FBalanceableFloat BalanceableFloatName;

	/**
	* Initializes a MIDIFloat in the Constructor of an Actor
	* @param BalanceableFloat - name you gave your MIDIFloat in the Headerfile
	* @param Value - DefaultValue for your MIDIFloat
	*/
	#define INITIALIZE_BALANCEABLEFLOAT(BalanceableFloat, Value) BalanceableFloat##Component = CreateDefaultSubobject<UBalanceableFloatComponent>(TEXT(#BalanceableFloat));\
		BalanceableFloat = FBalanceableFloat::NewFBalanceableFloat(Value);\
		BalanceableFloat##Component->FinishInitialization(\
			FLOAT_IDENTIFIER_FROM_STRING(#BalanceableFloat, this),\
			VARIABLENAME_FROM_STRING_AND_CLASS(#BalanceableFloat, this),\
			BalanceableFloat.GetStructPointer()\
		);\

	/**
	* Static conversion func for the blueprint version of MIDIFloatComponent.
	* It will be automatically used for conversion when you drag a MIDIFloatComponent into an float pin.
	* @brief Automatic cast to float
	* @param
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "MIDIFloat to Float Value", CompactNodeTitle = "->", BlueprintAutocast), Category = "MIDIVaraibles|Utility")
		static float Conv_MIDIFloatToFloat(UBalanceableFloatBP* UMIDIFloat);


//Begin: Utility MACROs

	/** Macro that fetches class name and user chosen name to "showname"*/
	#define VARIABLENAME_FROM_STRING_AND_CLASS(UserChosenName, Class)  Class->GetClass()->GetName() + '.' + FString(UserChosenName)
	
	/** Macro to generate an Identifier for a float from user chosen name*/
	#define FLOAT_IDENTIFIER_FROM_STRING(UserChosenName, Class) FILENAME+Class->GetClass()->GetName() + UserChosenName + "FLOAT"

	/** Macro to generate a Identifier for a float from user chosen name*/
	#define FILENAME UBalanceableVariablesLibrary::ToFileName(__FILE__)

	/**
	* Used in Almost all BalanceableVariable Macros
	* @param Path to the current file (generated through the __FILE__ Macro)
	* @return Name of the file when it is in a Public or private folder - alternatively the last 15 characters of the string excluding .cpp and .h
	*/
	static FString ToFileName(FString Path);
	
//End: Utility MACROs
};
