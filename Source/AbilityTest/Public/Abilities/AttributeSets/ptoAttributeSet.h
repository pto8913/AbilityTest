// Copyright（C）write by pto8913. 2020. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ptoAttributeSet.generated.h"

// Uses macros from AttributeSet.h
/* 
	指定されたクラスのプロパティに対する、
	セッター関数と、ゲッター関数を自動的に作ってくれるマクロ 
	GetHealthやSetHealth、GetHealthAttributeを使えるようになります
*/
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class ABILITYTEST_API UptoAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UptoAttributeSet();
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
		FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UptoAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category = "MaxHealth", ReplicatedUsing = OnRep_MaxHealth)
		FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UptoAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_Stamina)
		FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UptoAttributeSet, Stamina);

	UPROPERTY(BlueprintReadOnly, Category = "MaxStamina", ReplicatedUsing = OnRep_MaxStamina)
		FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UptoAttributeSet, MaxStamina);

	UPROPERTY(BlueprintReadOnly, Category = "MoveSpeed", ReplicatedUsing = OnRep_MoveSpeed)
		FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UptoAttributeSet, MoveSpeed)

	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_AttackPower)
		FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UptoAttributeSet, AttackPower)

	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_DefensePower)
		FGameplayAttributeData DefensePower;
	ATTRIBUTE_ACCESSORS(UptoAttributeSet, DefensePower)

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
		FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UptoAttributeSet, Damage);

protected:
	/* これらのOnRep関数は、変更の結果が、複製されるときに、指定されたクラス内の指定されたメンバ変数と適切に同期されているかを確認するためのもの */
	UFUNCTION()
		virtual void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
		virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
		virtual void OnRep_Stamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
		virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
		virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
		virtual void OnRep_AttackPower(const FGameplayAttributeData& OldValue);

	UFUNCTION()
		virtual void OnRep_DefensePower(const FGameplayAttributeData& OldValue);
};