// 

#pragma once

#include "CoreMinimal.h"
#include "EVGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "EVGA_Unequip.generated.h"

class UEVEquipmentManagerComponent;

/**
 * Universal unequip ability.
 * - Items with montage: plays montage, destroys actor at AnimNotify, removes GE/tags on montage end.
 * - Stat-only items: removes GE/tags instantly.
 */
UCLASS()
class EXVITIUM_API UEVGA_Unequip : public UEVGameplayAbility
{
	GENERATED_BODY()
	
public:
	UEVGA_Unequip();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	/** Full cleanup: removes GE + tags + clears entry. */
	void PerformUnequip();

	UFUNCTION()
	void OnUnequipMontageEnded();

	/** Called by AnimNotify during unequip montage — destroys actor. */
	UFUNCTION()
	void OnDespawnNotifyReceived(FGameplayEventData Payload);

	void FinishAbility(bool bWasCancelled);

	// --- Cached data ---
	
	FGameplayTag CachedSlot;

	UPROPERTY()
	TObjectPtr<UEVEquipmentManagerComponent> CachedEquipmentManager;

	bool bActorDestroyed = false;
};
