#include "GameplayTags/EVGameplayTags.h"

/* Abilities — identity tags for gameplay abilities, used for activation and mutual blocking */
namespace  EVTags::Abilities::Combat
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LightAttack, "EVTags.Abilities.Combat.LightAttack", "Light attack ability");
}

namespace  EVTags::Abilities::Equipment
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equip, "EVTags.Abilities.Equipment.Equip", "Equip ability");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Unequip, "EVTags.Abilities.Equipment.Unequip", "Unequip ability");
}


/* Events — tags sent via gameplay events to trigger abilities or signal AnimNotify timing */
namespace  EVTags::Events::Combat
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ChainAttack, "EVTags.Events.Combat.ChainAttack", "Triggers ChainAttack in LightAttack ability");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ChainAttackWindowOpened, "EVTags.Events.Combat.ChainAttackWindowOpened", "Triggers open chain in LightAttack ability for AnimNotifyState_ChainAttackWindow Begin");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ChainAttackWindowClosed, "EVTags.Events.Combat.ChainAttackWindowClosed", "Triggers close chain in LightAttack ability for AnimNotifyState_ChainAttackWindow End");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(WeaponCollisionStart, "EVTags.Events.Combat.WeaponCollisionStart", "Triggers WeaponCollisionStart in LightAttack ability");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(WeaponCollisionEnd, "EVTags.Events.Combat.WeaponCollisionEnd", "Triggers WeaponCollisionEnd in LightAttack ability");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(WeaponHit, "EVTags.Events.Combat.WeaponHit", "Triggers WeaponHit in any Attack ability");
}

namespace  EVTags::Events::Equipment
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equip, "EVTags.Events.Equipment.Equip", "Triggers equip ability");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Unequip, "EVTags.Events.Equipment.Unequip", "Triggers unequip ability");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(MontageNotify, "EVTags.Events.Equipment.MontageNotify", "AnimNotify signal for weapon spawn/despawn timing during equip/unequip montages");
}


/* Cues — gameplay cue tags for audio/VFX, assignable per item in EquipmentDefinition */
namespace  GameplayCue::Equipment
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(EquipRoot, "GameplayCue.Equipment.Equip", "Root equip cue");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UnequipRoot, "GameplayCue.Equipment.Unequip", "Root unequip cue");
}

namespace  GameplayCue::Equipment::Equip
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sword, "GameplayCue.Equipment.Equip.Sword", "Equip cue — Sword");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Longsword, "GameplayCue.Equipment.Equip.Longsword", "Equip cue — Longsword");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Bow, "GameplayCue.Equipment.Equip.Bow", "Equip cue — Bow");
}

namespace  GameplayCue::Equipment::Unequip
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sword, "GameplayCue.Equipment.Unequip.Sword", "Unequip cue — Sword");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Longsword, "GameplayCue.Equipment.Unequip.Longsword", "Unequip cue — Longsword");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Bow, "GameplayCue.Equipment.Unequip.Bow", "Unequip cue — Bow");
}


/* State — tags present on ASC representing current character state */
namespace EVTags::State
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dead, "EVTags.State.Dead", "Character is dead");
}

namespace EVTags::State::Equipped
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(MainHand, "EVTags.State.Equipped.MainHand", "MainHand slot is occupied");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(OffHand, "EVTags.State.Equipped.OffHand", "OffHand slot is occupied");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Head, "EVTags.State.Equipped.Head", "Head slot is occupied");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Chest, "EVTags.State.Equipped.Chest", "Chest slot is occupied");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Hands, "EVTags.State.Equipped.Hands", "Hands slot is occupied");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Feet, "EVTags.State.Equipped.Feet", "Feet slot is occupied");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ring, "EVTags.State.Equipped.Ring", "Ring slot is occupied");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Necklace, "EVTags.State.Equipped.Necklace", "Necklace slot is occupied");
}

namespace EVTags::State::Combat
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attacking, "EVTags.State.Combat.Attacking", "Character is performing an attack");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Blocking, "EVTags.State.Combat.Blocking", "Character is blocking");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Invulnerable, "EVTags.State.Combat.Invulnerable", "Character is invulnerable");
}

namespace EVTags::State::Equipment
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipping, "EVTags.State.Equipment.Equipping", "Character is equipping an item");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Unequipping, "EVTags.State.Equipment.Unequipping", "Character is unequipping an item");
}


/* EquipmentSlot — slot identifiers for the equipment system */
namespace EVTags::EquipmentSlot
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(MainHand, "EVTags.EquipmentSlot.MainHand", "MainHand equipment slot");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(OffHand, "EVTags.EquipmentSlot.OffHand", "OffHand equipment slot");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Head, "EVTags.EquipmentSlot.Head", "Head equipment slot");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Chest, "EVTags.EquipmentSlot.Chest", "Chest equipment slot");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Hands, "EVTags.EquipmentSlot.Hands", "Hands equipment slot");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Feet, "EVTags.EquipmentSlot.Feet", "Feet equipment slot");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ring, "EVTags.EquipmentSlot.Ring", "Ring equipment slot");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Necklace, "EVTags.EquipmentSlot.Necklace", "Necklace equipment slot");
}


/* EnemyType — enemy classification */
namespace EVTags::EnemyType
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Normal, "EVTags.EnemyType.Normal", "Normal enemy — receives hit reactions");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Epic, "EVTags.EnemyType.Epic", "Epic enemy");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Boss, "EVTags.EnemyType.Boss", "Boss enemy — ignores hit reactions");
}


/* WeaponType — weapon classification, auto-granted to ASC on equip. Drives animation state */
namespace EVTags::WeaponType
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(WeaponTypeRoot, "EVTags.WeaponType", "Root weapon type tag for animation system matching");
}

namespace EVTags::WeaponType::Melee
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sword, "EVTags.WeaponType.Melee.Sword", "Sword");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Longsword, "EVTags.WeaponType.Melee.Longsword", "Longsword");
}

namespace EVTags::WeaponType::Ranged
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Bow, "EVTags.WeaponType.Ranged.Bow", "Bow");
}


/* ItemCategory — item classification for inventory and filtering */
namespace EVTags::ItemCategory
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(EquipmentRoot, "EVTags.ItemCategory.Equipment", "Equipment item");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Valuable, "EVTags.ItemCategory.Valuable", "Valuable item — can be sold");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Consumable, "EVTags.ItemCategory.Consumable", "Consumable item — potions, bombs");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Crafting, "EVTags.ItemCategory.Crafting", "Crafting material");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Quest, "EVTags.ItemCategory.Quest", "Quest item");
}

namespace EVTags::ItemCategory::Equipment
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon, "EVTags.ItemCategory.Equipment.Weapon", "Weapon — swords, axes, bows");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Secondary, "EVTags.ItemCategory.Equipment.Secondary", "Secondary — shields");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Armor, "EVTags.ItemCategory.Equipment.Armor", "Armor — helmets, chests, boots, gloves");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Accessory, "EVTags.ItemCategory.Equipment.Accessory", "Accessory — rings, necklaces");
}
