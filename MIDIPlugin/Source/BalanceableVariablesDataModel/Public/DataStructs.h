// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataStructs.generated.h"

USTRUCT()
struct BALANCEABLEVARIABLESDATAMODEL_API FNamedId
{
	GENERATED_BODY()

	static FNamedId NewFNamedId(FString Name, FString Id);

	FORCEINLINE FString GetName() const;
	FORCEINLINE FString GetId() const;

	bool operator==(const FNamedId& Other) const { return this->Id.Equals(Other.GetId()); }

private:
	FString Name;
	FString Id;
};

USTRUCT()
struct BALANCEABLEVARIABLESDATAMODEL_API FBalancingData
{
	GENERATED_BODY()

	FNamedId NamedId;
	float CurrentValue;
	float LowerBound;
	float UpperBound;
};
