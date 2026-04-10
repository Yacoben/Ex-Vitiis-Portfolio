// 

#pragma once

#include "CoreMinimal.h"
#include "EVGameplayAbility.h"
#include "EVGA_LightAttack.generated.h"

class UAbilityTask_PlayMontageAndWait;

/**
 * 
 */
UCLASS()
class EXVITIUM_API UEVGA_LightAttack : public UEVGameplayAbility
{
	GENERATED_BODY()
	
public:
	UEVGA_LightAttack();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
								const FGameplayAbilityActorInfo* ActorInfo,
								const FGameplayAbilityActivationInfo ActivationInfo, 
								const FGameplayEventData* TriggerEventData) override;
	
	
private:
	// Variables
	UPROPERTY()
	TArray<TSoftObjectPtr<UAnimMontage>> AttackMontages;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> CurrentMontageTask = nullptr;
	
	int32 ComboIndex = 0;
	bool bChainAttackWindowOpened = false;
	
	// Functions
	void PlayAttackMontage(int32 MontageIndex);
	
	UFUNCTION()
	void OnComboMontageCompleted();
	
	UFUNCTION()
	void OnComboMontageCancelled();
	
	UFUNCTION()
	void OnChainWindowOpened(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnChainWindowClosed(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnChainInputReceived(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnWeaponHit(FGameplayEventData EventData);
	
	void ResetCombo();
	
	int32 GetEffectiveMaxChain() const;
};
