// Copyright Pirmin Pfeifer & Quirin Maier. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BalanceableFloatBundle.h"
#include "BalanceableFloatBP.generated.h"

/* Blueprint Version of UMIDIFloatComponent. */
UCLASS(ClassGroup = (BalanceableVariables), meta = (BlueprintSpawnableComponent))
class BALANCEABLEVARIABLES_API UBalanceableFloatBP : public UActorComponent, public IBalanceableFloatAccess
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "BalanceableVariables|Utility")
		UBalanceableFloatBP* Initialize(FString Identifier, float DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "BalanceableVariables|Utility")
		UBalanceableFloatBP* InitializeWithOwnName(float DefaultValue);
	virtual void DestroyComponent(bool bPromoteChildren = false) override;
	virtual void Activate(bool bReset) override;
	virtual void Deactivate() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
protected:
	UPROPERTY(BlueprintReadOnly)
	FString Identifier;
	FString Name;
	float Value = INFINITY;

public:
	//Begin: IBalanceableFloatAccess Interface

	/* Called Internally never call this function by your self
	 * @param NewValue new value for the float
	 */
	virtual void UpdateValue(float NewValue) override;

	UFUNCTION(BlueprintPure, Category = "BalanceableVariables|Getter")
		virtual FString GetIdentifier() const override;
	
	UFUNCTION(BlueprintPure, Category = "BalanceableVariables|Getter")
		virtual FString GetVariableName() const override;

	//End: IBalanceableFloatAccess Interface

	UFUNCTION(BlueprintPure, Category = "BalanceableVariables|Getter")
		float GetValue() const;
};
