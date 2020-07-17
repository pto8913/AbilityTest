// CopyrightÅiCÅjwrite by pto8913. 2020. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "ptoAT_Test.generated.h"

/** Delegate type used */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FptoTestDelegate);

UCLASS()
class ABILITYTEST_API UptoAT_Test : public UAbilityTask
{
	GENERATED_BODY()
public:
	UptoAT_Test();

	/* Activate Task */
	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;
	virtual void ExternalCancel() override;

	UPROPERTY(BlueprintAssignable)
		FptoTestDelegate OnCompleted;

	UPROPERTY(BlueprintAssignable)
		FptoTestDelegate OnCancelled;

	UPROPERTY(BlueprintAssignable)
		FptoTestDelegate OnBlendOut;

	UPROPERTY(BlueprintAssignable)
		FptoTestDelegate OnInterrupted;

	/*
	@ OwningAbility : A GameplayAbility that calls this task
	@ TaskInstanceName : The Instance name return by this func. need not be
	@ MontageToPlay : Animation Montage
	*/
	UFUNCTION(BlueprintCallable, Category = "Ability|pto|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UptoAT_Test* ptoTest(
			UGameplayAbility* OwningAbility,
			FName TaskInstanceName,
			UAnimMontage* MontageToPlay,
			float Rate = 1.f,
			FName StartSectionName = NAME_None,
			float AnimRootMotionTranslationScale = 1.f
		);

private:
	void OnMontageEnd(UAnimMontage* Montage, bool bInterrupted);
	void OnAbilityCancelled();
	bool StopPlayingMontage();
	void OnMontageBlendOut(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY()
		UAnimMontage* MontageToPlay;

	UPROPERTY()
		float Rate;

	UPROPERTY()
		FName StartSectionName;

	UPROPERTY()
		float AnimRootMotionTranslationScale;

	FOnMontageEnded MontageEndDelegate;
	FOnMontageBlendingOutStarted MontageBlendOutDelegate;

	FDelegateHandle CancelHandle;
};