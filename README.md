# **ExVitium** — Source Code Portfolio

> **Ex Vitium** is a first-person dungeon crawler game set in a dark fantasy world, combining immersive exploration, RPG systems, resource management, and challenging combat.
> This repository contains **source code only** — no assets, binaries, or third-party plugins.

---

## About the Project

**ExVitium** is a first-person melee combat game inspired by souls-like titles. The project is built entirely in C++ on top of Unreal Engine's **Gameplay Ability System (GAS)**, with a strong focus on clean architecture, data-driven design, and modular systems.

### Key Technical Features

- **Combo chain attack system** driven by AnimNotify windows and GAS events
- **Data-driven equipment system** with two-phase equip flow (data → visual)
- **Weapon trace hit detection** with multi-segment sweep between frames
- **Async asset preloading** — equipment assets are streamed on equip, not on use
- **Native Gameplay Tags** organized by domain (State, Abilities, Events, Equipment Slots)
- **Custom Object Pooling plugin** for efficient actor reuse

---

## Architecture Overview

```
Source/
├── ExVitium/                        # Main game module
│   ├── AbilitySystem/               # GAS: abilities, attribute sets, tasks
│   │   ├── Abilities/               # GA_LightAttack, GA_Equip, GA_Unequip
│   │   ├── AttributeSets/           # Vital (HP, Stamina), Combat (MaxChain)
│   │   └── Tasks/                   # Custom AbilityTask: WeaponTrace
│   ├── Actors/Weapons/              # Weapon actor (EVWeapon)
│   ├── Animation/                   # AnimInstance, AnimNotifies
│   ├── Characters/                  # Base → Player / Enemy hierarchy
│   ├── Components/                  # EquipmentManager, WeaponTrace, Footstep
│   ├── Game/                        # GameMode, GameState
│   ├── GameplayTags/                # All native gameplay tags
│   ├── Interfaces/                  # Equipment interfaces
│   ├── PDA/                         # Primary Data Assets: Item → Equipment → Weapon
│   └── Player/                      # PlayerController, PlayerState
├── ExVitiumEditor/                  # Editor-only module
Plugins/
└── ObjectPooling/                   # Custom reusable object pooling plugin
```

---

## Combat System

The combat system is built on the **Gameplay Ability System** with a montage-driven combo chain:

1. **`UEVGA_LightAttack`** — Instanced-per-actor ability that plays attack montages from the equipped weapon's definition
2. **Chain Attack Window** — AnimNotify states (`EVAnimNotifyState_ChainAttackWindow`) open/close input windows during montages
3. **Combo progression** — Player input during the chain window advances `ComboIndex` and plays the next montage
4. **Max chain limit** — Governed by `UEVCombatAttributes::MaxChainAttacks` attribute, allowing GE-based buffs to extend combos
5. **Weapon Trace** — `UEVWeaponTraceComponent` performs multi-segment sphere sweeps between frames for accurate hit detection

```
[Input] → GA_LightAttack → PlayMontage(0)
                              ├── AnimNotify: ChainWindowOpened
                              │     └── [Input] → PlayMontage(1) → ...
                              ├── AnimNotify: WeaponTraceStart
                              │     └── WeaponTraceComponent → OnHit
                              └── AnimNotify: ChainWindowClosed
```

---

## Equipment System

Equipment uses a **two-phase design** separating data from visuals:

| Phase | What happens | When |
|-------|-------------|------|
| **Data** (`EquipItem`) | Registers item, applies GE + tags, grants abilities, preloads assets | Immediately on equip ability activation |
| **Visual** (`SpawnEquippedActor`) | Spawns and attaches the 3D actor to skeleton | On AnimNotify during equip montage |

This split allows the weapon to appear at the exact right animation frame while the gameplay effects are already active.

### Data Asset Hierarchy

```
UEVItemDefinition (Primary Data Asset)
  └── UEVEquipmentDefinition (slots, GE, visuals, montages, weight)
        └── UEVWeaponDefinition (weapon type, attack montages, damage, stamina cost)
```

Each definition declares `GetAssetsToPreload()` for async streaming and `CollectGrantedTags()` for tag composition.

---

## Object Pooling Plugin

A standalone **editor plugin** (`Plugins/ObjectPooling/`) providing a reusable `UObjectPoolingComponent`:

- Pre-spawns a configurable number of actors on `BeginPlay`
- `AcquireObject(Transform)` / `ReturnObject(Actor)` API
- Actors are parked at a configurable off-screen location when inactive
- Blueprint-assignable delegates: `OnPoolInitialized`, `OnAcquireObject`, `OnReturnObject`
- Implements `IObjectPoolingInterface` for pooled actors to handle activation/deactivation callbacks

---

## Tech Stack

| | |
|---|---|
| **Engine** | Unreal Engine 5.7 |
| **Language** | C++ |
| **Core Systems** | Gameplay Ability System (GAS), Enhanced Input, Animation Montages |
| **Audio** | FMOD Studio (third-party, not included) |
| **Source Control** | Git + LFS |

---

## Note

This is a **source-code-only** portfolio repository. It is not intended to compile standalone — it requires Unreal Engine 5.7, project assets, and third-party plugins (FMOD Studio) that are not included.  
The code is shared to demonstrate architecture, C++ proficiency, and Unreal Engine systems knowledge.

---

## License

This code is shared for **portfolio and review purposes only**.  
All rights reserved. Do not use in commercial or personal projects without permission.

