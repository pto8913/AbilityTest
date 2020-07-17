// Copyright（C）write by pto8913. 2020. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "CharacterBase.generated.h"

class UGameplayAbility;
class UAbilitySystemComponent;
class UptoAttributeSet;
class UGameplayEffect;

UCLASS()
class ABILITYTEST_API ACharacterBase 
	: public ACharacter, public IAbilitySystemInterface
{
	GENERATED_UCLASS_BODY()
public:
	// Begin Init
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability")
		UAbilitySystemComponent* AbilitySystemComp;
	/* Implement IAbilitySystemInterface */
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
		TArray<TSubclassOf<UGameplayAbility>> GameplayAbilities;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
		TArray<TSubclassOf<UGameplayAbility>> PassiveGameplayAbilities;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
		TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;
	void AddStartupGameplayAbilities();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
		FGameplayTag MyTag;

	UFUNCTION()
		void OnCompHit(
			UPrimitiveComponent* HitComponent, 
			AActor* OtherActor, 
			UPrimitiveComponent* OtherComp, 
			FVector NormalImpulse, 
			const FHitResult& Hit
		);

protected:
	UPROPERTY(EditAnywhere, Replicated, Category = "Ability")
		int32 ExistsLevel = 1;

	UPROPERTY()
		int32 bAbilitiesInitialized;

	UPROPERTY()
		UptoAttributeSet* AttributeSet;
	// End Init

	UFUNCTION(BlueprintImplementableEvent)
		void OnStaminaChanged(float DeltaValue, const FGameplayTagContainer& EventTags);
	UFUNCTION(BlueprintImplementableEvent)
		void OnDefensePowerChanged(float DeltaValue, const FGameplayTagContainer& EventTags);
	UFUNCTION(BlueprintImplementableEvent)
		void OnHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags);
	UFUNCTION(BlueprintImplementableEvent)
		void OnDamage(
			float DamageAmount,
			const FHitResult& HitInfo,
			const struct FGameplayTagContainer& DamageTags,
			ACharacterBase* InstigatorPawn,
			AActor* DamageCauser
		);

public:
	UFUNCTION(BlueprintCallable)
		virtual float GetStamina() const;
	UFUNCTION(BlueprintCallable)
		virtual float GetMaxStamina() const;
	UFUNCTION(BlueprintCallable)
		virtual float GetDefensePower() const;
	UFUNCTION(BlueprintCallable)
		virtual float GetHealth() const;
	UFUNCTION(BlueprintCallable)
		virtual float GetMaxHealth() const;

	// ptoAttributeSetから呼ばれて、上記のOn〇〇ChangedがBPから呼ばれます
	virtual void HandleStaminaChanged(float DeltaValue, const FGameplayTagContainer& EventTags);
	virtual void HandleDefensePowerChanged(float DeltaValue, const FGameplayTagContainer& EventTags);
	virtual void HandleHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags);
	virtual void HandleDamage(
		float DamageAmount, 
		const FHitResult& HitInfo, 
		const struct FGameplayTagContainer& DamageTags, 
		ACharacterBase* InstigatorPawn, 
		AActor* DamageCauser
	);

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};