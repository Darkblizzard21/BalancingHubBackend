// Copyright Pirmin Pfeifer & Quirin Maier. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/UnrealTemplate.h"
/**
 * This is a collection of all Reqeuset Sturcts
 */

 /**
 * Struct that is used for storing float update requests
 */
template<typename VariableType>
struct TUpdateRequest
{

public:
	/**
	* Used to get a NewFFloatUpdateRequest
	* @param Identifier - of the float that should be changed
	* @param NewValue - updated Value
	* @returns the new  FFloatUpdateRequest
	*/
	static TUpdateRequest NewTUpdateRequest(FString Identifier, VariableType NewValue)
	{
		TUpdateRequest Request = TUpdateRequest();
		Request.Value = NewValue;
		Request.Identifier = Identifier;

		return Request;
	}

	/** Makes Value Accessible - the Value Never Changes*/
	VariableType GetValue() { return Value; };

	/** Makes Identifier Accessible - the Identifier Never Changes*/
	FString GetIdentifier() const { return Identifier; };

private:
	VariableType Value;
	FString Identifier;
};