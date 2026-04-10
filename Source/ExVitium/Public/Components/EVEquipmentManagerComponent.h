// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "Components/ActorComponent.h"
#include "Engine/StreamableManager.h"
#include "EVEquipmentManagerComponent.generated.h"

class UEVEquipmentDefinition;
class UAbilitySystemComponent;
class USkeletalMeshComponent;

/**
 * Represents a single equipped item in a slot.
 */
USTRUCT(BlueprintType)
struct FEVEquippedItemEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<const UEVEquipmentDefinition> Definition = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> SpawnedActor = nullptr;

	FActiveGameplayEffectHandle ActiveEffectHandle;

	FGameplayTagContainer AppliedTags;

	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;

	/** Keeps preloaded assets (montages, meshes) alive while item is equipped. Released on unequip. */
	TSharedPtr<FStreamableHandle> PreloadHandle;

	bool IsValid() const { return Definition != nullptr; }

	void Reset()
	{
		Definition = nullptr;
		SpawnedActor = nullptr;
		ActiveEffectHandle = FActiveGameplayEffectHandle();
		AppliedTags.Reset();
		GrantedAbilityHandles.Empty();
		PreloadHandle.Reset();
	}
};

/**
 * Manages equipped items per slot. Handles spawning/destroying actors, applying/removing GE, granting/removing tags.
 * Lives on AEVPlayerCharacter. Abilities delegate equip/unequip data logic to this component.
 *
 * Equip is split into two phases:
 *  - Data phase (EquipItem): registers item, applies GE + tags. No actor spawned.
 *  - Visual phase (SpawnEquippedActor): spawns and attaches the 3D actor.
 *  This split allows abilities to spawn the actor at the right montage moment via AnimNotify.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class EXVITIUM_API UEVEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEVEquipmentManagerComponent();

	/**
	 * DATA PHASE: Registers item in slot, applies GE + tags. Does NOT spawn an actor.
	 * If slot is already occupied, automatically unequips old item first.
	 * Call SpawnEquippedActor() separately to create the visual.
	 */
	bool EquipItem(
		const UEVEquipmentDefinition* Definition,
		FGameplayTag Slot,
		UAbilitySystemComponent* ASC);

	/**
	 * VISUAL PHASE: Spawns and attaches the 3D actor for the item currently in the given slot.
	 * No-op if slot is empty, item has no SpawnableActorClass, or actor already exists.
	 */
	bool SpawnEquippedActor(
		FGameplayTag Slot,
		USkeletalMeshComponent* TargetMesh,
		USkeletalMeshComponent* ShadowMesh = nullptr);

	/**
	 * Destroys only the visual actor for the given slot. Entry and GE/tags remain.
	 * Used by unequip montage notify — weapon disappears mid-animation.
	 */
	void DestroyEquippedActor(FGameplayTag Slot);

	/**
	 * Full unequip: destroys actor (if exists) + removes GE + tags + clears entry.
	 */
	bool UnequipItem(FGameplayTag Slot, UAbilitySystemComponent* ASC);

	/** Returns true if the given slot has an item equipped. */
	UFUNCTION(BlueprintCallable, Category = "EV|Equipment")
	bool IsSlotOccupied(FGameplayTag Slot) const;

	/** Returns the equipped item entry for the given slot, or nullptr if empty. */
	const FEVEquippedItemEntry* GetEquippedItem(FGameplayTag Slot) const;

	/** Returns all currently equipped items. */
	const TMap<FGameplayTag, FEVEquippedItemEntry>& GetAllEquippedItems() const { return EquippedItems; }
	
	// Returns sum weight of all equipped items 
	UFUNCTION(BlueprintCallable, Category = "EV|Equipment")
	float GetWeightAllEquippedItems() const;
	

private:
	UPROPERTY()
	TMap<FGameplayTag, FEVEquippedItemEntry> EquippedItems;
	
	UPROPERTY(VisibleAnywhere, Category = "EV|Equipment|Weight")
	float CachedCurrentWeight = 0.f;

	/** Maps EVTags.EquipmentSlot.X → EVTags.State.Equipped.X */
	static FGameplayTag GetEquippedStateTag(const FGameplayTag& SlotTag);
};
