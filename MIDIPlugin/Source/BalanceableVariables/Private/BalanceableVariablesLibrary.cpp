// Copyright Pirmin Pfeifer. All Rights Reserved.


#include "BalanceableVariablesLibrary.h"

float UBalanceableVariablesLibrary::Conv_MIDIFloatToFloat(UBalanceableFloatBP* UMIDIFloat)
{
	return UMIDIFloat->GetValue();
}

FString UBalanceableVariablesLibrary::ToFileName(const FString Path)
{
	const int32 Length = Path.Len();
	//Find right end cut
	int32 EndCut = 4;//Default .cpp
	if (Path.EndsWith(".h"))
	{
		EndCut = 2;
	}
	//Check for private folder
	int32 StartIndex = Path.Find("private", ESearchCase::IgnoreCase, ESearchDir::FromStart);
	if (StartIndex != INDEX_NONE)
	{
		StartIndex += 8;
		return Path.Mid(StartIndex, Length - StartIndex - EndCut);
	}
	//Check for public folder
	StartIndex = Path.Find("public", ESearchCase::IgnoreCase, ESearchDir::FromStart);
	if (StartIndex != INDEX_NONE)
	{
		StartIndex += 7;
		return Path.Mid(StartIndex, Length - StartIndex - EndCut);
	}
	return Path.Mid(Length - 16, 15 - EndCut);
}
