// Copyright Pirmin Pfeifer. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "BalanceableFloatComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBalanceableFloatTestStatndartOperator,
                                 "GameMixerTests.BalanceableVariables.BalanceableFloat.StandartOperatorTest",
                                 EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::ProductFilter)

bool FBalanceableFloatTestStatndartOperator::RunTest(const FString& Parameters)
{
	//Initialize Float
	const float Value1 = 3.f;
	const float Value2 = 2.f;
	const FBalanceableFloat BalanceableFloat1 = FBalanceableFloat::NewFBalanceableFloat(Value1);
	const FBalanceableFloat BalanceableFloat2 = FBalanceableFloat::NewFBalanceableFloat(Value2);

	// ReSharper disable four CppJoinDeclarationAndAssignment
	float ExpectedResult;
	float LeftResult;
	float RightResult;
	float SelfResult;
	//Test every float math operator
	//Test Addition
	ExpectedResult = Value1 + Value2;
	LeftResult = BalanceableFloat1 + Value2;
	RightResult = Value1 + BalanceableFloat2;
	SelfResult = BalanceableFloat1 + BalanceableFloat2;
	TestEqual("Check if BalanceableFloat can be left part of an add operation between floats", ExpectedResult,
	          LeftResult);
	TestEqual("Check if BalanceableFloat can be right part of an add operation between floats", ExpectedResult,
	          RightResult);
	TestEqual("Check if addition works between BalanceableFloats", ExpectedResult, SelfResult);

	//Test Subtraction
	ExpectedResult = Value1 - Value2;
	LeftResult = BalanceableFloat1 - Value2;
	RightResult = Value1 - BalanceableFloat2;
	SelfResult = BalanceableFloat1 - BalanceableFloat2;
	TestEqual("Check if BalanceableFloat can be left part of an subtract operation between floats", ExpectedResult,
	          LeftResult);
	TestEqual("Check if BalanceableFloat can be right part of an subtract operation between floats", ExpectedResult,
	          RightResult);
	TestEqual("Check if subtraction works between BalanceableFloats", ExpectedResult, SelfResult);

	//Test Multiplication
	ExpectedResult = Value1 * Value2;
	LeftResult = BalanceableFloat1 * Value2;
	RightResult = Value1 * BalanceableFloat2;
	SelfResult = BalanceableFloat1 * BalanceableFloat2;
	TestEqual("Check if BalanceableFloat can be left part of an multiplication operation between floats",
	          ExpectedResult, LeftResult);
	TestEqual("Check if BalanceableFloat can be right part of an multiplication operation between floats",
	          ExpectedResult, RightResult);
	TestEqual("Check if multiplication works between BalanceableFloats", ExpectedResult, SelfResult);

	//Test Division
	ExpectedResult = Value1 / Value2;
	LeftResult = BalanceableFloat1 / Value2;
	RightResult = Value1 / BalanceableFloat2;
	SelfResult = BalanceableFloat1 / BalanceableFloat2;
	TestEqual("Check if BalanceableFloat can be left part of an division operation between floats", ExpectedResult,
	          LeftResult);
	TestEqual("Check if BalanceableFloat can be right part of an division operation between floats", ExpectedResult,
	          RightResult);
	TestEqual("Check if division works between BalanceableFloats", ExpectedResult, SelfResult);

	// Return true to finish your test
	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBalanceableFloatTestAdvancedOperator,
                                 "GameMixerTests.BalanceableVariables.BalanceableFloat.AdvancedOperatorTest",
                                 EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::ProductFilter)

bool FBalanceableFloatTestAdvancedOperator::RunTest(const FString& Parameters)
{
	//Initialize Variables
	const float Value1 = 3.f;
	const FBalanceableFloat BalanceableFloat1 = FBalanceableFloat::NewFBalanceableFloat(Value1);
	const FVector Vector = FVector(1.f, 2.f, 3.f);
	const FVector2D Vector2D = FVector2D(1.f, 2.f);
	const FVector4 Vector4 = FVector4(1.f, 2.f, 3.f, 4.f);


	//Tests
	const FVector ExpectedResultV3 = Value1 * Vector;
	const FVector BalanceableFloatResultV3 = BalanceableFloat1 * Vector;
	TestEqual("Check Vector Multiplication", ExpectedResultV3, BalanceableFloatResultV3);

	const FVector2D ExpectedResultV2 = Value1 * Vector2D;
	const FVector2D BalanceableFloatResultV2 = BalanceableFloat1 * Vector2D;
	TestEqual("Check Vector2D Multiplication", ExpectedResultV2, BalanceableFloatResultV2);

	const FVector4 ExpectedResultV4 = Value1 * Vector4;
	const FVector4 BalanceableFloatResultV4 = BalanceableFloat1 * Vector4;
	TestEqual("Check Vector4 Multiplication", ExpectedResultV4, BalanceableFloatResultV4);

	// Return true to finish your test
	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBalanceableFloatTestBooleanOperator,
                                 "GameMixerTests.BalanceableVariables.BalanceableFloat.BooleanOperatorTest",
                                 EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::ProductFilter)

bool FBalanceableFloatTestBooleanOperator::RunTest(const FString& Parameters)
{
	//Initialize Float
	const float Value1 = 2.f;
	const float Value2 = 2.f;
	const float ValueSmall = 1.f;
	const float ValueBig = 3.f;
	const FBalanceableFloat BalanceableFloat1 = FBalanceableFloat::NewFBalanceableFloat(Value1);
	const FBalanceableFloat BalanceableFloat2 = FBalanceableFloat::NewFBalanceableFloat(Value2);
	const FBalanceableFloat BalanceableFloatBig = FBalanceableFloat::NewFBalanceableFloat(ValueBig);

	//Test every boolean operator
	//Test Equal
	bool LeftResult = BalanceableFloat1 == Value2;
	bool RightResult = Value1 == BalanceableFloat2;
	bool SelfResult = BalanceableFloat1 == BalanceableFloat2;
	TestTrue("Check if a BalanceableFloat with the same value produces true as the left side of an == operator",
	         LeftResult);
	TestTrue("Check if a BalanceableFloat with the same value produces true as the right side of an == operator",
	         RightResult);
	TestTrue("Check if two BalanceableFloats with the same value produces true when compared using the == operator",
	         SelfResult);

	//Test !Equal
	LeftResult = BalanceableFloat1 != Value2;
	RightResult = Value1 != BalanceableFloat2;
	SelfResult = BalanceableFloat1 != BalanceableFloat2;
	TestFalse("Check if a BalanceableFloat with the same value produces false as the left side of an != operator",
	          LeftResult);
	TestFalse("Check if a BalanceableFloat with the same value produces false as the right side of an != operator",
	          RightResult);
	TestFalse("Check if two BalanceableFloats with the same value produces false when compared using the != operator",
	          SelfResult);

	//Test
	LeftResult = BalanceableFloat1 < ValueBig;
	RightResult = ValueSmall < BalanceableFloat2;
	SelfResult = BalanceableFloat1 < BalanceableFloatBig;
	TestTrue("Check if a BalanceableFloat works with < as left side of the operator", LeftResult);
	TestTrue("Check if a BalanceableFloat works with < as left side of the operator", RightResult);
	TestTrue("Check if a BalanceableFloat works with <", SelfResult);

	//Test >
	LeftResult = BalanceableFloat1 > ValueSmall;
	RightResult = ValueBig > BalanceableFloat2;
	SelfResult = BalanceableFloatBig > BalanceableFloat2;
	TestTrue("Check if a BalanceableFloat works with > as left side of the operator", LeftResult);
	TestTrue("Check if a BalanceableFloat works with > as left side of the operator", RightResult);
	TestTrue("Check if a BalanceableFloat works with >", SelfResult);

	//Test <=
	LeftResult = BalanceableFloat1 <= ValueBig;
	RightResult = ValueSmall <= BalanceableFloat2;
	SelfResult = BalanceableFloat1 <= BalanceableFloatBig;
	TestTrue("Check if a BalanceableFloat works with <= as left side of the operator", LeftResult);
	TestTrue("Check if a BalanceableFloat works with <= as left side of the operator", RightResult);
	TestTrue("Check if a BalanceableFloat works with <=", SelfResult);

	//Test >=
	LeftResult = BalanceableFloat1 >= ValueSmall;
	RightResult = ValueBig >= BalanceableFloat2;
	SelfResult = BalanceableFloatBig >= BalanceableFloat2;
	TestTrue("Check if a BalanceableFloat works with >= as left side of the operator", LeftResult);
	TestTrue("Check if a BalanceableFloat works with >= as left side of the operator", RightResult);
	TestTrue("Check if a BalanceableFloat works with >=", SelfResult);
	// Return true to finish your test
	return true;
}
