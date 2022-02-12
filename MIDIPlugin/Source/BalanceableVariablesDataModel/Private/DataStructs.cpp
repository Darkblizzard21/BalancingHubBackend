// Fill out your copyright notice in the Description page of Project Settings.


#include "DataStructs.h"

FNamedId FNamedId::NewFNamedId(const FString Name, const FString Id)
{
	FNamedId Result = FNamedId();
	Result.Id = Id;
	Result.Name = Name;
	return Result;
}

FString FNamedId::GetName() const
{
	return Name;
}

FString FNamedId::GetId() const
{
	return Id;
}
