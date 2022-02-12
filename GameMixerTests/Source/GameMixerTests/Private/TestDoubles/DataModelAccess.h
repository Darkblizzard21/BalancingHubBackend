// Copyright Pirmin Pfeifer. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataModel.h"

/**
 * 
 */
class GAMEMIXERTESTS_API FDataModelAccess final : public FDataModel
{

	public:
	void OverrideFreeVariables(const TArray<FNamedId> Data) 
	{
		FreeVariables = Data;
	}

	TArray<FNamedId> GetData() const
	{
		return FreeVariables;
	}
};
