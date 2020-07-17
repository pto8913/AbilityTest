// CopyrightÅiCÅjwrite by pto8913. 2020. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_Stamina.generated.h"

UCLASS()
class ABILITYTEST_API UMMC_Stamina : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UMMC_Stamina();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

	UPROPERTY()
		FGameplayEffectAttributeCaptureDefinition StaminaDef;

	UPROPERTY()
		FGameplayEffectAttributeCaptureDefinition MaxStaminaDef;
};