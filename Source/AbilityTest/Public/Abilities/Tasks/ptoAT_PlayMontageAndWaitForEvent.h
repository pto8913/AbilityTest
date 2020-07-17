// CopyrightÅiCÅjwrite by pto8913. 2020. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "ptoAT_PlayMontageAndWaitForEvent.generated.h"

/** Delegate type used, EventTag and Payload may be empty if it came from the montage callbacks */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FptoPlayMontageAndWaitForEventDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

UCLASS()
class ABILITYTEST_API UptoAT_PlayMontageAndWaitForEvent : public UAbilityTask
{
	GENERATED_BODY()
public:
	UptoAT_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;
	virtual void ExternalCancel() override;

	/* The montage completely finished playing */
	UPROPERTY(BlueprintAssignable)
		FptoPlayMontageAndWaitForEventDelegate OnCompleted;

	/* The montage started blending out */
	UPROPERTY(BlueprintAssignable)
		FptoPlayMontageAndWaitForEventDelegate OnBlendOut;

	/* The montage was interrupted */
	UPROPERTY(BlueprintAssignable)
		FptoPlayMontageAndWaitForEventDelegate OnInterrupted;

	/* The ability task was explicitly cancelled by another ability */
	UPROPERTY(BlueprintAssignable)
		FptoPlayMontageAndWaitForEventDelegate OnCancelled;

	/* One of the triggering gameplay events happened */
	UPROPERTY(BlueprintAssignable)
		FptoPlayMontageAndWaitForEventDelegate EventReceived;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UptoAT_PlayMontageAndWaitForEvent* PlayMontageAndWaitForEvent(
			UGameplayAbility* OwningAbility,
			FName TaskInstanceName,
			UAnimMontage* MontageToPlay,
			FGameplayTagContainer EventTags,
			float Rate = 1.f,
			FName StartSection = NAME_None,
			bool bMontageStopWhenAbilityEnds = true,
			float AnimRootMotionTranslationScale = 1.f
		);

private:
	/** Montage that is playing */
	UPROPERTY()
		UAnimMontage* MontageToPlay;

	/** List of tags to match against gameplay events */
	UPROPERTY()
		FGameplayTagContainer EventTags;

	/** Playback rate */
	UPROPERTY()
		float Rate;

	/** Section to start montage from */
	UPROPERTY()
		FName StartSection;

	/** Modifies how root motion movement to apply */
	UPROPERTY()
		float AnimRootMotionTranslationScale;

	/** Rather montage should be aborted if ability ends */
	UPROPERTY()
		bool bMontageStopWhenAbilityEnds;

	bool StopPlayingMontage();

	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	void OnAbilityCancelled();
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle CancelHandle;
	FDelegateHandle EventHandle;

};
