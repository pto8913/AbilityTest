// CopyrightÅiCÅjwrite by pto8913. 2020. All Rights Reserved.

#include "Abilities/Calculations/MMC_Cooldown.h"

#include "Abilities/Ability/ptoGameplayAbility.h"

float UMMC_Cooldown::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const UptoGameplayAbility* Ability = Cast<UptoGameplayAbility>(
		Spec.GetContext().GetAbilityInstance_NotReplicated()
	);

	if (!Ability)
	{
		return 0.0f;
	}

	return Ability->CooldownDuration.GetValueAtLevel(Ability->GetAbilityLevel());
}