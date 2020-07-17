// CopyrightÅiCÅjwrite by pto8913. 2020. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_Cooldown.generated.h"

UCLASS()
class ABILITYTEST_API UMMC_Cooldown : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};