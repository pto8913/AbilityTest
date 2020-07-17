// CopyrightiCjwrite by pto8913. 2020. All Rights Reserved.

#include "Abilities/Calculations/EC_DamageWithDFSDebuff.h"

#include "Abilities/AttributeSets/ptoAttributeSet.h"
#include "AbilitySystemComponent.h"

struct ptoDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Stamina);

	ptoDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UptoAttributeSet, DefensePower, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UptoAttributeSet, AttackPower, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UptoAttributeSet, Damage, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UptoAttributeSet, Stamina, Source, true);
	}
};

static const ptoDamageStatics& DamageStatics()
{
	static ptoDamageStatics DamageStatics;
	return DamageStatics;
}

UEC_DamageWithDFSDebuff::UEC_DamageWithDFSDebuff()
{
	RelevantAttributesToCapture.Add(DamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().DefensePowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().StaminaDef);
}

void UEC_DamageWithDFSDebuff::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	OUT FGameplayEffectCustomExecutionOutput& OutExecutionParams
) const
{
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceASC ? SourceASC->AvatarActor : nullptr;
	AActor* TargetActor = TargetASC ? TargetASC->AvatarActor : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float DefensePower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().DefensePowerDef,
		EvaluationParameters,
		DefensePower
	);

	float AttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().AttackPowerDef,
		EvaluationParameters,
		AttackPower
	);

	float Damage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().DamageDef,
		EvaluationParameters,
		Damage
	);

	float Stamina = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().StaminaDef,
		EvaluationParameters,
		Stamina
	);

	float DamageDone = Damage * (AttackPower - DefensePower);
	if (DamageDone > 0.f)
	{
		OutExecutionParams.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				DamageStatics().DamageProperty,
				EGameplayModOp::Additive,
				DamageDone
			)
		);

		OutExecutionParams.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				DamageStatics().StaminaProperty,
				EGameplayModOp::Additive,
				Stamina
			)
		);
	}
}