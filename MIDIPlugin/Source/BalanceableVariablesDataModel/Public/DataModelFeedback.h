// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum EDataModelError
{
	None,
	IdentifierNotPresentInData,
	IdentifierAlreadyExists,
	CantRemoveElement,
	UnknownERR 
};

template <typename T>
struct TDataModelFeedback
{
	static TDataModelFeedback<T> CreateSuccessFeedback(T Data, FString Message = "")
	{
		TDataModelFeedback<T> Result = TDataModelFeedback<T>();
		Result.Data = Data;
		Result.Message = Message;
		Result.Error = EDataModelError::None;
		return Result;
	}

	static TDataModelFeedback<T> CreateErrorFeedback(EDataModelError Error, FString Message)
	{
		TDataModelFeedback<T> Result = TDataModelFeedback<T>();
		Result.Error = Error == EDataModelError::None ? EDataModelError::UnknownERR : Error;
		Result.Message = Message;
		return Result;
	}

	bool Success() const { return Error == EDataModelError::None; }
	EDataModelError GetError() const { return Error; }
	FString GetMessage() const { return Message; }

	const T GetDataOrThrow() const
	{
		if (!Success()) throw "No Data found to get in \"GetOrThrow\" function.";
		return Data;
	}

	const T* const GetDataPtrOrThrow() const
	{
		if (!Success()) throw "No Data found to get in \"GetOrThrow\" function.";
		return &Data;
	}

	bool TryGetData(T& DataOut) const
	{
		const bool bSuccess = Success();
		if(bSuccess) DataOut = Data;
		return bSuccess;
	}

	// Casts TDataModelFeedback into another type
	// only save for error feedbacks
	template <typename F>
	const TDataModelFeedback<F> ExplicitCastErrorOrThrow() const
	{
		if (Success()) throw "Cant cast Success fulls feedback Data into another class";;
		return TDataModelFeedback<F>::CreateErrorFeedback(Error, Message);
	}

private:
	FString Message;
	EDataModelError Error = EDataModelError::None;
	T Data;
};
