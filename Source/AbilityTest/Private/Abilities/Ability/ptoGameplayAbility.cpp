// CopyrightÅiCÅjwrite by pto8913. 2020. All Rights Reserved.

#include "Abilities/Ability/ptoGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"

#include "AbilitySystemBlueprintLibrary.h"

UptoGameplayAbility::UptoGameplayAbility()
{
	// Default to Instance Per Actor
	//InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// Default tags that block this ability from activating
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")));
}

//UGameplayEffect* UptoGameplayAbility::GetCostGameplayEffect() const
//{
//	UGameplayEffect* NewGE = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("Bounty")));
//	NewGE->DurationPolicy = EGameplayEffectDurationType::Instant;
//
//	int32 Idx = NewGE->Modifiers.Num();
//	NewGE->Modifiers.SetNum(Idx + 1);
//
//	FGameplayModifierInfo& InfoXP = NewGE->Modifiers[Idx];
//	InfoXP.ModifierMagnitude = FScalableFloat(Cost);
//	InfoXP.ModifierOp = EGameplayModOp::Additive;
//	InfoXP.Attribute = UGDAttributeSetBase::GetXPAttribute();
//
//	ApplyGameplayEffectToSelf(NewGE, 1.0f, Source->MakeEffectContext());
//}

void UptoGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (ActivateAbilityOnGranted)
	{
		bool ActivatedAbility = ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}


const FGameplayTagContainer* UptoGameplayAbility::GetCooldownTags() const
{
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&Temp_CooldownTags);
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags)
	{
		MutableTags->AppendTags(*ParentTags);
	}
	MutableTags->AppendTags(CooldownTags);
	return MutableTags;
}

void UptoGameplayAbility::ApplyCooldown(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo
) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}