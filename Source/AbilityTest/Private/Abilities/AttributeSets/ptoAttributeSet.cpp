// Copyright（C）write by pto8913. 2020. All Rights Reserved.

#include "Abilities/AttributeSets/ptoAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Characters/Bases/CharacterBase.h"
#include "Net/UnrealNetWork.h"

UptoAttributeSet::UptoAttributeSet() 
	: Health(1.f)
	, MaxHealth(1)
	, Stamina(1.f)
	, MaxStamina(1.f)
	, MoveSpeed(1.f)
	, AttackPower(1.f)
	, DefensePower(1.f)
	, Damage(0.f)
{
}

void UptoAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UptoAttributeSet, Health);
	DOREPLIFETIME(UptoAttributeSet, MaxHealth);
	DOREPLIFETIME(UptoAttributeSet, Stamina);
	DOREPLIFETIME(UptoAttributeSet, MaxStamina);
	DOREPLIFETIME(UptoAttributeSet, AttackPower);
	DOREPLIFETIME(UptoAttributeSet, DefensePower);
	DOREPLIFETIME(UptoAttributeSet, MoveSpeed);
}

void UptoAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	float DeltaValue = 0.f;
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		DeltaValue = Data.EvaluatedData.Magnitude;
	}

	/* これはこのAttributeSetのオーナーであるはずです */
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	ACharacterBase* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<ACharacterBase>(TargetActor);
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		AActor* SourceActor = nullptr;
		AController* SourceController = nullptr;
		ACharacterBase* SourceCharacter = nullptr;
		if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
		{
			SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
			SourceController = Source->AbilityActorInfo->PlayerController.Get();
			if (SourceController == nullptr && SourceActor != nullptr)
			{
				if (APawn* Pawn = Cast<APawn>(SourceActor))
				{
					SourceController = Pawn->GetController();
				}
			}

			if (SourceController)
			{
				SourceCharacter = Cast<ACharacterBase>(SourceController->GetPawn());
			}
			else
			{
				SourceCharacter = Cast<ACharacterBase>(SourceActor);
			}
		}

		FHitResult HitResult;
		if (Context.GetHitResult())
		{
			HitResult = *Context.GetHitResult();
		}

		const float LocalDamageDone = GetDamage();

		SetDamage(0.f);

		if (LocalDamageDone > 0.f)
		{
			const float OldHealth = GetHealth();
			SetHealth(FMath::Clamp(OldHealth - LocalDamageDone, 0.f, GetMaxHealth()));

			if (TargetCharacter)
			{
				TargetCharacter->HandleDamage(
					LocalDamageDone, HitResult, SourceTags, SourceCharacter, SourceActor
				);

				TargetCharacter->HandleHealthChanged(
					-LocalDamageDone, SourceTags
				);
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetDefensePowerAttribute())
	{
		SetDefensePower(FMath::Clamp(GetDefensePower(), 0.f, GetDefensePower()));

		if (IsValid(TargetCharacter) == true)
		{
			TargetCharacter->HandleDefensePowerChanged(DeltaValue, SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));

		if (IsValid(TargetCharacter) == true)
		{
			TargetCharacter->HandleHealthChanged(DeltaValue, SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		SetMaxHealth(FMath::Clamp(GetMaxHealth(), 0.f, GetMaxHealth()));

		if (IsValid(TargetCharacter) == true)
		{
			TargetCharacter->HandleHealthChanged(DeltaValue, SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));

		if (IsValid(TargetCharacter) == true)
		{
			TargetCharacter->HandleStaminaChanged(DeltaValue, SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetMaxStaminaAttribute())
	{
		SetMaxStamina(FMath::Clamp(GetMaxStamina(), 0.f, GetMaxStamina()));

		if (IsValid(TargetCharacter) == true)
		{
			TargetCharacter->HandleStaminaChanged(DeltaValue, SourceTags);
		}
	}
}

void UptoAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UptoAttributeSet, Health, OldValue);
}

void UptoAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UptoAttributeSet, MaxHealth, OldValue);
}

void UptoAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UptoAttributeSet, Stamina, OldValue);
}

void UptoAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UptoAttributeSet, MaxStamina, OldValue);
}

void UptoAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UptoAttributeSet, MoveSpeed, OldValue);
}

void UptoAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UptoAttributeSet, AttackPower, OldValue);
}

void UptoAttributeSet::OnRep_DefensePower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UptoAttributeSet, DefensePower, OldValue);
}