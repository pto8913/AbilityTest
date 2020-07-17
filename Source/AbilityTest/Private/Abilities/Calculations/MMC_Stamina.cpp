// CopyrightÅiCÅjwrite by pto8913. 2020. All Rights Reserved.

#include "Abilities/Calculations/MMC_Stamina.h"
#include "GameplayEffectTypes.h"
#include "Abilities/AttributeSets/ptoAttributeSet.h"

UMMC_Stamina::UMMC_Stamina()
{
	StaminaDef.AttributeToCapture = UptoAttributeSet::GetStaminaAttribute();
	StaminaDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	StaminaDef.bSnapshot = false;

	MaxStaminaDef.AttributeToCapture = UptoAttributeSet::GetMaxStaminaAttribute();
	MaxStaminaDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	MaxStaminaDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(StaminaDef);
	RelevantAttributesToCapture.Add(MaxStaminaDef);
}

#include "Abilities/Ability/ptoGameplayAbility.h"

float UMMC_Stamina::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	//const UGameplayAbility* Ability = Cast<UGameplayAbility>(Spec.GetContext().GetAbilityInstance_NotReplicated());

	float Stamina = 0.f;
	GetCapturedAttributeMagnitude(StaminaDef, Spec, EvaluationParameters, Stamina);
	Stamina = FMath::Max<float>(Stamina, 0.f);

	float MaxStamina = 0.f;
	GetCapturedAttributeMagnitude(MaxStaminaDef, Spec, EvaluationParameters, MaxStamina);
	MaxStamina = FMath::Max<float>(MaxStamina, 1.f);

	float Res = 0.f;

	const UptoGameplayAbility* Ability = Cast<UptoGameplayAbility>(Spec.GetContext().GetAbilityInstance_NotReplicated());

	if (IsValid(Ability) == true)
	{
		Res = Ability->Cost.GetValueAtLevel(Ability->GetAbilityLevel());
	}

	if (Stamina / MaxStamina > 0.5f)
	{
		Res *= 2.f;
	}

	return Res;
}