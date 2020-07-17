// CopyrightÅiCÅjwrite by pto8913. 2020. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "EC_DamageWithDFSDebuff.generated.h"

UCLASS()
class ABILITYTEST_API UEC_DamageWithDFSDebuff : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UEC_DamageWithDFSDebuff();
	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		OUT FGameplayEffectCustomExecutionOutput& OutExecutionParams
	) const override;
};