// CopyrightÅiCÅjwrite by pto8913. 2020. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ptoGameplayAbility.generated.h"

UCLASS()
class ABILITYTEST_API UptoGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UptoGameplayAbility();

	// Begin Costs
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Costs")
		FScalableFloat Cost;
	//virtual UGameplayEffect* GetCostGameplayEffect() const override;
	// End Costs

	// Begin Cooldowns
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cooldowns")
		FScalableFloat CooldownDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cooldowns")
		FGameplayTagContainer CooldownTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cooldowns")
		FGameplayTagContainer Temp_CooldownTags;

	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	virtual void ApplyCooldown(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo
	) const override;
	// End Cooldowns

	// Tells an ability to activate immediately when its granted. Used for passive abilities and abilities forced on others.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
		bool ActivateAbilityOnGranted = false;

	// If an ability is marked as 'ActivateAbilityOnGranted', activate them immediately when given here
	// Epic's comment: Projects may want to initiate passives or do other "BeginPlay" type of logic here.
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};