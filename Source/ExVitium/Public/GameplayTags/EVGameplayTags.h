#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

/* Abilities — AbilityTags on GA classes, used by BlockAbilitiesWithTag for mutual blocking */
namespace  EVTags::Abilities::Combat
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(LightAttack);
}

namespace  EVTags::Abilities::Equipment
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equip);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Unequip);
}

/* Events — gameplay event tags for HandleGameplayEvent and AnimNotify signals */
namespace  EVTags::Events::Combat
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ChainAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ChainAttackWindowOpened);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ChainAttackWindowClosed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(WeaponCollisionStart);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(WeaponCollisionEnd);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(WeaponHit);
}

namespace  EVTags::Events::Equipment
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equip);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Unequip);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MontageNotify);
}

/* Cues — gameplay cue tags assignable per item in EquipmentDefinition. NOT YET FIRED */
namespace  GameplayCue::Equipment
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipRoot);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UnequipRoot);
}

namespace  GameplayCue::Equipment::Equip
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sword);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Longsword);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Bow);
}

namespace  GameplayCue::Equipment::Unequip
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sword);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Longsword);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Bow);
}

/* State — tags on ASC via ActivationOwnedTags or LooseGameplayTags */
namespace EVTags::State
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dead);
}

namespace EVTags::State::Equipped
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MainHand);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(OffHand);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Head);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Chest);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hands);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Feet);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ring);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Necklace);
}

namespace EVTags::State::Combat
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attacking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Blocking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Invulnerable);
}

namespace EVTags::State::Equipment
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipping);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Unequipping);
}

/* EnemyType — enemy classification. Not yet used */
namespace EVTags::EnemyType
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Normal);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Epic);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Boss);
}

/* WeaponType — granted to ASC via CollectGrantedTags, listened by EVAnimInstance */
namespace EVTags::WeaponType
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(WeaponTypeRoot);
}

namespace EVTags::WeaponType::Melee
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sword);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Longsword);
}

namespace EVTags::WeaponType::Ranged
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Bow);
}

/* ItemCategory — classification for inventory filtering (future) */
namespace EVTags::ItemCategory
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentRoot);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Valuable);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Consumable);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crafting);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Quest);
}

namespace EVTags::ItemCategory::Equipment
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Secondary);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Armor);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Accessory);
}

/* EquipmentSlot — slot identifiers for EquipmentManager, CheatManager, FootstepComponent */
namespace EVTags::EquipmentSlot
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MainHand);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(OffHand);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Head);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Chest);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hands);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Feet);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ring);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Necklace);
}
