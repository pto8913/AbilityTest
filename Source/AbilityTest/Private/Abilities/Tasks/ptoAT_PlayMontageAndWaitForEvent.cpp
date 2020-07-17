// CopyrightÅiCÅjwrite by pto8913. 2020. All Rights Reserved.

#include "Abilities/Tasks/ptoAT_PlayMontageAndWaitForEvent.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

#include "GameFramework/Character.h"

UptoAT_PlayMontageAndWaitForEvent::UptoAT_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	Rate = 1.f;
	bMontageStopWhenAbilityEnds = true;
}

UptoAT_PlayMontageAndWaitForEvent* UptoAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	UAnimMontage* MontageToPlay,
	FGameplayTagContainer EventTags,
	float Rate,
	FName StartSection,
	bool bMontageStopWhenAbilityEnds,
	float AnimRootMotionTranslationScale
)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	UptoAT_PlayMontageAndWaitForEvent* MyObj = NewAbilityTask<UptoAT_PlayMontageAndWaitForEvent>(OwningAbility, TaskInstanceName);

	MyObj->MontageToPlay = MontageToPlay;
	MyObj->EventTags = EventTags;
	MyObj->Rate = Rate;
	MyObj->StartSection = StartSection;
	MyObj->bMontageStopWhenAbilityEnds = bMontageStopWhenAbilityEnds;
	MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	return MyObj;
}

void UptoAT_PlayMontageAndWaitForEvent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Ability && Ability->GetCurrentMontage() == MontageToPlay)
	{
		if (Montage == MontageToPlay)
		{
			AbilitySystemComponent->ClearAnimatingAbility(Ability);

			ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
			if (Character 
				&& 
				(
					Character->GetLocalRole() == ROLE_Authority 
					||
					(
						Character->GetLocalRole() == ROLE_AutonomousProxy 
						&& 
						Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted
					)
				)
			)
			{
				Character->SetAnimRootMotionTranslationScale(1.f);
			}
		}
	}

	if (bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void UptoAT_PlayMontageAndWaitForEvent::OnAbilityCancelled()
{
	if (StopPlayingMontage())
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void UptoAT_PlayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	EndTask();
}

void UptoAT_PlayMontageAndWaitForEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData EventData = *Payload;
		EventData.EventTag = EventTag;

		EventReceived.Broadcast(EventTag, EventData);
	}
}

void UptoAT_PlayMontageAndWaitForEvent::Activate()
{
	if (Ability == nullptr) return;

	bool bPlayedMontage = false;
	if (AbilitySystemComponent)
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			EventHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(
				EventTags,
				FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
					this,
					&UptoAT_PlayMontageAndWaitForEvent::OnGameplayEvent
				)
			);

			if (
				AbilitySystemComponent->PlayMontage(
					Ability,
					Ability->GetCurrentActivationInfo(),
					MontageToPlay,
					Rate,
					StartSection
				) > 0.f
			)
			{
				if (!ShouldBroadcastAbilityTaskDelegates()) return;

				CancelHandle = Ability->OnGameplayAbilityCancelled.AddUObject(
					this, 
					&UptoAT_PlayMontageAndWaitForEvent::OnAbilityCancelled
				);

				BlendingOutDelegate.BindUObject(
					this,
					&UptoAT_PlayMontageAndWaitForEvent::OnMontageBlendingOut
				);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(
					this,
					&UptoAT_PlayMontageAndWaitForEvent::OnMontageEnded
				);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
				if (
					Character && (Character->GetLocalRole() == ROLE_Authority ||
						(Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)
						)
					)
				{
					Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
				}

				bPlayedMontage = true;
			}
		}
	}

	if (!bPlayedMontage)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	SetWaitingOnAvatar();
}

void UptoAT_PlayMontageAndWaitForEvent::ExternalCancel()
{
	check(AbilitySystemComponent);

	OnAbilityCancelled();

	Super::ExternalCancel();
}

void UptoAT_PlayMontageAndWaitForEvent::OnDestroy(bool AbilityEnded)
{
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelHandle);
		if (AbilityEnded && bMontageStopWhenAbilityEnds)
		{
			StopPlayingMontage();
		}
	}

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
	}

	Super::OnDestroy(AbilityEnded);
}

bool UptoAT_PlayMontageAndWaitForEvent::StopPlayingMontage()
{
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo)
	{
		return false;
	}

	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return false;
	}

	// Check if the montage is still playing
	// The ability would have been interrupted, in which case we should automatically stop the montage
	if (AbilitySystemComponent && Ability)
	{
		if (AbilitySystemComponent->GetAnimatingAbility() == Ability
			&& AbilitySystemComponent->GetCurrentMontage() == MontageToPlay)
		{
			// Unbind delegates so they don't get called as well
			FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
			if (MontageInstance)
			{
				MontageInstance->OnMontageBlendingOutStarted.Unbind();
				MontageInstance->OnMontageEnded.Unbind();
			}

			AbilitySystemComponent->CurrentMontageStop();
			return true;
		}
	}

	return false;
}