// CopyrightÅiCÅjwrite by pto8913. 2020. All Rights Reserved.

#include "Abilities/Tasks/ptoAT_Test.h"

#include "AbilitySystemComponent.h"
#include "Engine.h"

UptoAT_Test::UptoAT_Test()
{
	Rate = 1.f;
}

UptoAT_Test* UptoAT_Test::ptoTest(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	UAnimMontage* MontageToPlay,
	float Rate,
	FName StartSectionName,
	float AnimRootMotionTranslationScale
)
{
	UptoAT_Test* MyObj = NewAbilityTask<UptoAT_Test>(
		OwningAbility, TaskInstanceName
	);

	MyObj->MontageToPlay = MontageToPlay;
	MyObj->Rate = Rate;
	MyObj->StartSectionName = StartSectionName;
	MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;

	return MyObj;
}

void UptoAT_Test::Activate()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, "Activate in ptoAT_Test");
	if (!Ability) return;
	if (!AbilitySystemComponent) return;

	bool bPlayedMontage = false;

	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (AnimInstance)
	{
		AbilitySystemComponent->PlayMontage(
			Ability,
			Ability->GetCurrentActivationInfo(),
			MontageToPlay,
			Rate,
			StartSectionName
		);

		CancelHandle = Ability->OnGameplayAbilityCancelled.AddUObject(
			this,
			&UptoAT_Test::OnAbilityCancelled
		);

		MontageBlendOutDelegate.BindUObject(
			this,
			&UptoAT_Test::OnMontageBlendOut
		);
		AnimInstance->Montage_SetBlendingOutDelegate(MontageBlendOutDelegate, MontageToPlay);

		MontageEndDelegate.BindUObject(
			this,
			&UptoAT_Test::OnMontageEnd
		);
		AnimInstance->Montage_SetEndDelegate(MontageEndDelegate, MontageToPlay);

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

void UptoAT_Test::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);
}

void UptoAT_Test::ExternalCancel()
{
	check(AbilitySystemComponent);

	OnAbilityCancelled();

	Super::ExternalCancel();
}

void UptoAT_Test::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast();
		}
	}
	
	EndTask();
}

void UptoAT_Test::OnAbilityCancelled()
{
	if (StopPlayingMontage())
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast();
		}
	}
}

bool UptoAT_Test::StopPlayingMontage()
{
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo) return false;

	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (!AnimInstance) return false;

	if (AbilitySystemComponent && Ability)
	{
		if ((AbilitySystemComponent->GetAnimatingAbility() == Ability)
			&& (AbilitySystemComponent->GetCurrentMontage() == MontageToPlay)
			)
		{
			FAnimMontageInstance* MontageInst = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
			if (MontageInst)
			{
				MontageInst->OnMontageEnded.Unbind();
			}

			AbilitySystemComponent->CurrentMontageStop();
			return true;
		}
	}

	return false;
}

void UptoAT_Test::OnMontageBlendOut(UAnimMontage* Montage, bool bInterrupted)
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
			OnInterrupted.Broadcast();
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnBlendOut.Broadcast();
		}
	}
}