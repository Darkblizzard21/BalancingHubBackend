// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Templates/SharedPointer.h"


struct FNamedId;
struct FBalancingData;
class FJsonObject;

class FDataModelLoadingHelper final
{
public:
	static bool LoadActiveBalancingPerComposition(TSharedPtr<FJsonObject> RootObj, TMap<FString, FString>& OutMap);
	static bool LoadBalanceableVariableDataForBalancingInComposition(
		TSharedPtr<FJsonObject> RootObj,
		TMap<FString, TMap<FString, TArray<TSharedRef<FBalancingData>>>>& OutMap);
	static bool LoadBalanceableVariablesInComposition(
		TSharedPtr<FJsonObject> RootObj,
			TMap<FString, TArray<FNamedId>>& OutMap);
};
