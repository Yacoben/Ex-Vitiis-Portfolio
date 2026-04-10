// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "EVItemDefinition.h"
#include "GameplayCueInterface.h"
#include "EVEquipmentDefinition.generated.h"

class UAnimMontage;
class UStaticMesh;
class USkeletalMesh;
class UGameplayEffect;
class UGameplayAbility;

/**
 * Equipment definition class. Contains properties related to unequippable and equippable items: spawnable actor, attach socket, meshes, equip/unequip animations
 * Items with SpawnableActorClass spawn a 3D actor (weapons, shields) but items without are stat-only (armor, accessories)
 */
UCLASS()
class EXVITIUM_API UEVEquipmentDefinition : public UEVItemDefinition
{
	GENERATED_BODY()

public:
	UEVEquipmentDefinition();

	// Stats
	
	// Item weight
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Stats")
	float Weight = 0.f;
	
	
	// Slot
	
	// Equipment slots this item is allowed to occupy (e.g. EquipmentSlot.MainHand)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Tags|Equipment", meta = (ToolTip = "Equipment slots this item is allowed to occupy (e.g. EquipmentSlot.MainHand)"))
	FGameplayTagContainer AllowedSlots;
	
	// Tags granted to ASC while equipped. Removed on unequip. These tags are considered as extra tags e.g. Special.GoodShit
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Tags|Equipment", meta = (Tooltip = "Tags granted to ASC while equipped. Removed on unequip. These tags are considered as extra e.g. Special.GoodShit"))
	FGameplayTagContainer GrantedTags;

	
	// Gameplay Effects
	
	// Infinite-duration Gameplay Effect applied while this item is equipped (stats, bonuses). Removed on unequip
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|GameplayEffects", meta = (Tooltip = "Infinite-duration Gameplay Effect applied while this item is equipped (stats, bonuses). Removed on unequip"))
	TSubclassOf<UGameplayEffect> OnEquipEffect;
	
	// Gameplay Abilities granted while this item is equipped. Removed on unequip
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|GameplayAbilities", meta = (Tooltip = "Gameplay Abilities granted while this item is equipped. Removed on unequip"))
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;
	
	
	// Visuals
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Visuals")
	TSoftObjectPtr<UStaticMesh> EquipmentMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Visuals")
	TSoftObjectPtr<USkeletalMesh> EquipmentSkeletalMesh;
	
	// Actor class to spawn for 3D representation. nullptr = stat-only item (no visual)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Visuals", meta = (ToolTipt = "Actor class to spawn for 3D representation. nullptr = stat-only item (no visual)"))
	TSubclassOf<AActor> SpawnableActorClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Visuals")
	FName AttachSocketName;

	
	// Animations
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Animations")
	TSoftObjectPtr<UAnimMontage> EquipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Animations")
	TSoftObjectPtr<UAnimMontage> UnequipMontage;
	
	
	// Cues
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Tags|Cues")
	FGameplayCueTag EquipCueTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Tags|Cues")
	FGameplayCueTag UnequipCueTag;

	/**
	 * Collects all soft asset paths that should be preloaded when this item is equipped.
	 * Override in subclasses to add type-specific assets (e.g. attack montages for weapons).
	 */
	virtual void GetAssetsToPreload(TArray<FSoftObjectPath>& OutPaths) const;

	/**
	 * Collects all gameplay tags that should be granted to ASC when this item is equipped.
	 * Base returns GrantedTags. Subclasses override to add type-specific tags (e.g. WeaponType).
	 * GrantedTags are used for extra optional tags
	 */
	virtual void CollectGrantedTags(FGameplayTagContainer& OutTags) const;
};

