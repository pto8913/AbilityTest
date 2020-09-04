// CopyrightÅiCÅjwrite by pto8913. 2020. All Rights Reserved.

#include "Abilities/Characters/Bases/CharacterBase.h"
#include "Abilities/AttributeSets/ptoAttributeSet.h"

#include "AbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "GameplayTagContainer.h"

#include "Components/CapsuleComponent.h"

/////////////////////////////////////////
// Init

// Sets default values
ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilityComp"));
	AbilitySystemComp->ReplicationMode = EGameplayEffectReplicationMode::Minimal;

	AttributeSet = CreateDefaultSubobject<UptoAttributeSet>(TEXT("AttributeSet"));

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ACharacterBase::OnCompHit);
}

void ACharacterBase::OnCompHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit
)
{
	UE_LOG(LogTemp, Log, TEXT("Hit"));

	//UAbilityTask

	MyTag = FGameplayTag::RequestGameplayTag(FName("Ability.Ability1"));
	UE_LOG(LogTemp, Log, TEXT("%s"), *MyTag.GetTagName().ToString());
	

	//FGameplayTag::RequestGameplayTag();
	FGameplayEventData EventData;
	EventData.Target = OtherActor;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, MyTag, EventData);
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComp;

	//AbilitySystemComp->GetActivatableAbilities();

	//FGameplayTagContainer Container;
	//AbilitySystemComp->GetOwnedGameplayTags(Container);
	//AbilitySystemComp->GetActivatableGameplayAbilitySpecsByAllMatchingTags();
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (IsValid(AbilitySystemComp) == true)
	{
		AbilitySystemComp->InitAbilityActorInfo(this, this);
		AddStartupGameplayAbilities();
	}
}

void ACharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (IsValid(AbilitySystemComp) == true)
	{
		AbilitySystemComp->RefreshAbilityActorInfo();
	}
}

void ACharacterBase::AddStartupGameplayAbilities()
{
	if (IsValid(AbilitySystemComp) == true)
	{
		for (TSubclassOf<UGameplayAbility>& StartupAbility : GameplayAbilities)
		{
			AbilitySystemComp->GiveAbility(FGameplayAbilitySpec(StartupAbility, ExistsLevel, INDEX_NONE, this));
		}

		for (TSubclassOf<UGameplayAbility>& PassiveAbility : PassiveGameplayAbilities)
		{
			FGameplayAbilitySpec Spec = FGameplayAbilitySpec(PassiveAbility, ExistsLevel, INDEX_NONE, this);
			AbilitySystemComp->GiveAbilityAndActivateOnce(Spec);
		}

		FGameplayEffectContextHandle EffectContext = AbilitySystemComp->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		for (TSubclassOf<UGameplayEffect> GameplayEffect : PassiveGameplayEffects)
		{
			FGameplayEffectSpecHandle NewHandle = AbilitySystemComp->MakeOutgoingSpec(GameplayEffect, ExistsLevel, EffectContext);
			if (NewHandle.IsValid())
			{
				AbilitySystemComp->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComp);
			}
		}

		bAbilitiesInitialized = true;
	}
}

float ACharacterBase::GetStamina() const 
{
	if (IsValid(AttributeSet))
	{
		return AttributeSet->GetStamina();
	}
	UE_LOG(LogTemp, Log, TEXT("AttributeSet is null in Stamina"));
	return 1.f;
}

float ACharacterBase::GetMaxStamina() const 
{
	if (IsValid(AttributeSet))
	{
		return AttributeSet->GetMaxStamina();
	}
	UE_LOG(LogTemp, Log, TEXT("AttributeSet is null in MaxStamina"));
	return 1.f;
}

float ACharacterBase::GetHealth() const
{
	if (IsValid(AttributeSet))
	{
		return AttributeSet->GetHealth();
	}
	UE_LOG(LogTemp, Log, TEXT("AttributeSet is null in Health"));
	return 1.f;
}

float ACharacterBase::GetMaxHealth() const
{
	if (IsValid(AttributeSet))
	{
		return AttributeSet->GetMaxHealth();
	}
	UE_LOG(LogTemp, Log, TEXT("AttributeSet is null in MaxHealth"));
	return 1.f;
}

float ACharacterBase::GetDefensePower() const
{
	if (IsValid(AttributeSet))
	{
		return AttributeSet->GetDefensePower();
	}
	UE_LOG(LogTemp, Log, TEXT("AttributeSet is null in DFS"));
	return 1.f;
}

void ACharacterBase::HandleStaminaChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	if (bAbilitiesInitialized)
	{
		OnStaminaChanged(DeltaValue, EventTags);
	}
}

void ACharacterBase::HandleDefensePowerChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	if (bAbilitiesInitialized)
	{
		OnDefensePowerChanged(DeltaValue, EventTags);
	}
}

void ACharacterBase::HandleHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	if (bAbilitiesInitialized)
	{
		OnHealthChanged(DeltaValue, EventTags);
	}
}

void ACharacterBase::HandleDamage(
	float DamageAmount, 
	const FHitResult& HitInfo, 
	const struct FGameplayTagContainer& DamageTags, 
	ACharacterBase* InstigatorPawn, 
	AActor* DamageCauser
)
{
	OnDamage(DamageAmount, HitInfo, DamageTags, InstigatorPawn, DamageCauser);
}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

////////////////////////
