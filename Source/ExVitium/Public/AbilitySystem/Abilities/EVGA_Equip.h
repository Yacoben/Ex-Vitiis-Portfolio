// 

#pragma once

#include "CoreMinimal.h"
#include "EVGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "EVGA_Equip.generated.h"

class UEVEquipmentDefinition;
class UEVEquipmentManagerComponent;
class USkeletalMeshComponent;

/**
 * Universal equip ability. Handles all equipment types:
 * - Weapons/Shields: data (GE+tags) immediately, actor spawned at AnimNotify, montage plays
 * - Armor/Accessories: data only (no actor, no montage)
 * 
 * If the target slot is occupied, plays unequip montage first (swap flow).
 */
UCLASS()
class EXVITIUM_API UEVGA_Equip : public UEVGameplayAbility
{
	GENERATED_BODY()
	
public:
	UEVGA_Equip();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	/** Phase 2: registers new item (GE+tags), starts equip montage or finishes. */
	void PerformEquip();

	// --- Montage callbacks ---
	
	UFUNCTION()
	void OnUnequipMontageEnded();

	UFUNCTION()
	void OnEquipMontageEnded();

	// --- AnimNotify callbacks (spawn/despawn at the right montage frame) ---

	/** Called by AnimNotify during OLD item's unequip montage — destroys old weapon. */
	UFUNCTION()
	void OnDespawnNotifyReceived(FGameplayEventData Payload);

	/** Called by AnimNotify during NEW item's equip montage — spawns new weapon. */
	UFUNCTION()
	void OnSpawnNotifyReceived(FGameplayEventData Payload);

	void FinishAbility(bool bWasCancelled);

	// --- Cached data for async montage flow ---
	
	UPROPERTY()
	TObjectPtr<const UEVEquipmentDefinition> CachedDefinition;

	FGameplayTag CachedSlot;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> CachedTargetMesh;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> CachedShadowMesh;

	UPROPERTY()
	TObjectPtr<UEVEquipmentManagerComponent> CachedEquipmentManager;

	/** Tracks if actor was already spawned/destroyed by notify (safety for interrupted montages). */
	bool bActorSpawned = false;
	bool bActorDestroyed = false;
};
