// 


#include "Cheats/EVCheatManager.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/EVPlayerCharacter.h"
#include "Components/EVEquipmentManagerComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTags/EVGameplayTags.h"
#include "Interfaces/EVEquipmentInterface.h"
#include "PDA/EVEquipmentDefinition.h"
#include "AbilitySystem/AttributeSets/EVCombatAttributes.h"
#include "Components/EVWeaponTraceComponent.h"

/*
 * Class for cheats only.
 */

UEVCheatManager::UEVCheatManager()
{
	RegisterConsoleCommands();
}

void UEVCheatManager::BeginDestroy()
{
	UnregisterConsoleCommands();
	Super::BeginDestroy();
}

void UEVCheatManager::RegisterConsoleCommands()
{
	auto RegisterCmd = [this](const TCHAR* Name, const TCHAR* Help, FConsoleCommandWithArgsDelegate Delegate)
	{
		// Unregister stale command from previous CheatManager instance (PIE restart, level transition)
		if (IConsoleObject* Existing = IConsoleManager::Get().FindConsoleObject(Name))
		{
			IConsoleManager::Get().UnregisterConsoleObject(Existing);
		}
		RegisteredCommands.Add(
			IConsoleManager::Get().RegisterConsoleCommand(Name, Help, MoveTemp(Delegate), ECVF_Default));
	};

	RegisterCmd(
		TEXT("Equipment.Equip"),
		TEXT("Equip an item by Data Asset name. Usage: Equipment.Equip <DA_Name>"),
		FConsoleCommandWithArgsDelegate::CreateUObject(this, &UEVCheatManager::HandleEquip));

	RegisterCmd(
		TEXT("Equipment.Unequip"),
		TEXT("Unequip a slot by name. Usage: Equipment.Unequip <SlotName> (e.g. MainHand, Head, Chest)"),
		FConsoleCommandWithArgsDelegate::CreateUObject(this, &UEVCheatManager::HandleUnequip));

	RegisterCmd(
		TEXT("Equipment.Print"),
		TEXT("Print all currently equipped items."),
		FConsoleCommandWithArgsDelegate::CreateUObject(this, &UEVCheatManager::HandlePrintEquippedItems));

	RegisterCmd(
		TEXT("Combat.SetMaxChainAttacks"),
		TEXT("Set MaxChainAttacks attribute value. Usage: Combat.SetMaxChainAttacks <Value>"),
		FConsoleCommandWithArgsDelegate::CreateUObject(this, &UEVCheatManager::HandleSetMaxChainAttacksLevel));

	RegisterCmd(
		TEXT("Combat.ShowDebug"),
		TEXT("Enable weapon trace debug visualization."),
		FConsoleCommandWithArgsDelegate::CreateUObject(this, &UEVCheatManager::HandleShowCombatDebug));

	RegisterCmd(
		TEXT("Combat.HideDebug"),
		TEXT("Disable weapon trace debug visualization."),
		FConsoleCommandWithArgsDelegate::CreateUObject(this, &UEVCheatManager::HandleHideCombatDebug));
}

void UEVCheatManager::UnregisterConsoleCommands()
{
	for (IConsoleObject* Cmd : RegisteredCommands)
	{
		if (Cmd)
		{
			IConsoleManager::Get().UnregisterConsoleObject(Cmd);
		}
	}
	RegisteredCommands.Empty();
}

// Equipment.Equip <DA_Name> 

void UEVCheatManager::HandleEquip(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Usage: Equipment.Equip <DA_Name>"));
		return;
	}

	ACharacter* Character = GetPlayerController() ? GetPlayerController()->GetCharacter() : nullptr;
	if (!Character) return;

	const FString& ItemDAName = Args[0];

	static const TArray<FString> SearchPaths = {
		TEXT("/Game/ExVitium/Design/DataAssets/Items/Weapons/"),
		TEXT("/Game/ExVitium/Design/DataAssets/Items/Chests/"),
		TEXT("/Game/ExVitium/Design/DataAssets/Items/Hands/"),
		TEXT("/Game/ExVitium/Design/DataAssets/Items/Rings/"),
		TEXT("/Game/ExVitium/Design/DataAssets/Items/Feet/"),
		TEXT("/Game/ExVitium/Design/DataAssets/Items/"),
	};

	UEVEquipmentDefinition* EquipmentDefinition = nullptr;

	for (const FString& BasePath : SearchPaths)
	{
		const FString FullPath = BasePath + ItemDAName + TEXT(".") + ItemDAName;
		UObject* Loaded = StaticLoadObject(UEVItemDefinition::StaticClass(), nullptr, *FullPath);
		if (Loaded)
		{
			EquipmentDefinition = Cast<UEVEquipmentDefinition>(Loaded);
			break;
		}
	}

	if (!EquipmentDefinition)
	{
		UE_LOG(LogTemp, Warning, TEXT("Equipment.Equip: Could not find equipment definition '%s'."), *ItemDAName);
		return;
	}

	if (EquipmentDefinition->AllowedSlots.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Equipment.Equip: '%s' has no allowed slots defined."), *ItemDAName);
		return;
	}

	const FGameplayTag Slot = EquipmentDefinition->AllowedSlots.GetByIndex(0);
	const bool bResult = IEVEquipmentInterface::Execute_TryEquip(Character, EquipmentDefinition, Slot);

	if (bResult)
	{
		UE_LOG(LogTemp, Log, TEXT("Equipment.Equip: '%s' equipped to slot '%s'."), *ItemDAName, *Slot.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Equipment.Equip: Failed to equip '%s'. Make sure equip ability is granted."), *ItemDAName);
	}
}

// Equipment.Unequip <SlotName>

void UEVCheatManager::HandleUnequip(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Usage: Equipment.Unequip <SlotName> (e.g. MainHand, Head, Chest)"));
		return;
	}

	ACharacter* Character = GetPlayerController() ? GetPlayerController()->GetCharacter() : nullptr;
	if (!Character) return;

	const FString FullEquipmentSlot = TEXT("EVTags.EquipmentSlot.") + Args[0];
	const FGameplayTag FullEquipmentTag = FGameplayTag::RequestGameplayTag(*FullEquipmentSlot, false);

	if (!FullEquipmentTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Equipment.Unequip: Slot '%s' does not exist."), *FullEquipmentSlot);
		return;
	}

	const bool bResult = IEVEquipmentInterface::Execute_TryUnequip(Character, FullEquipmentTag);

	if (bResult)
	{
		UE_LOG(LogTemp, Log, TEXT("Equipment.Unequip: Unequipped from slot '%s'."), *FullEquipmentTag.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Equipment.Unequip: Failed to unequip from slot '%s'. Make sure unequip ability is granted."), *FullEquipmentTag.ToString());
	}
}

// Equipment.Print

void UEVCheatManager::HandlePrintEquippedItems(const TArray<FString>& Args)
{
	const AEVPlayerCharacter* PlayerCharacter = GetPlayerController()
		? Cast<AEVPlayerCharacter>(GetPlayerController()->GetCharacter())
		: nullptr;
	
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Equipment.Print: EVPlayerCharacter is nullptr."));
		return;
	}

	const UEVEquipmentManagerComponent* EquipmentManager = PlayerCharacter->FindComponentByClass<UEVEquipmentManagerComponent>();
	if (!EquipmentManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("Equipment.Print: Player character does not have an equipment manager component."));
		return;
	}

	TArray<FString> EquippedItems;
	for (const auto& Pair : EquipmentManager->GetAllEquippedItems())
	{
		const FString ItemName = Pair.Value.Definition ? Pair.Value.Definition->GetName() : TEXT("null");
		EquippedItems.Add(FString::Printf(TEXT("  %s: %s"), *Pair.Key.ToString(), *ItemName));
	}

	if (EquippedItems.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Equipment.Print: No items equipped."));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Equipment.Print: Currently equipped:\n%s"), *FString::Join(EquippedItems, TEXT("\n")));
	}
}

// Combat.SetMaxChainAttacks <Value>

void UEVCheatManager::HandleSetMaxChainAttacksLevel(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Usage: Combat.SetMaxChainAttacks <Value>"));
		return;
	}

	const float NewValue = FCString::Atof(*Args[0]);
	if (NewValue < 1.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Combat.SetMaxChainAttacks: Value must be >= 1."));
		return;
	}

	APawn* Pawn = GetPlayerController() ? GetPlayerController()->GetPawn() : nullptr;
	if (!Pawn) return;

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn);
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("Combat.SetMaxChainAttacks: No AbilitySystemComponent found."));
		return;
	}

	ASC->SetNumericAttributeBase(UEVCombatAttributes::GetMaxChainAttacksAttribute(), NewValue);
	UE_LOG(LogTemp, Log, TEXT("Combat.SetMaxChainAttacks: Set to %.0f."), NewValue);
}

// Combat.ShowDebug

void UEVCheatManager::HandleShowCombatDebug(const TArray<FString>& Args)
{
	ACharacter* Character = GetPlayerController() ? GetPlayerController()->GetCharacter() : nullptr;
	if (!Character) return;

	const UEVEquipmentManagerComponent* EquipmentManager = Character->FindComponentByClass<UEVEquipmentManagerComponent>();
	if (!EquipmentManager) return;

	for (const auto& Pair : EquipmentManager->GetAllEquippedItems())
	{
		if (!IsValid(Pair.Value.SpawnedActor)) continue;
		if (UEVWeaponTraceComponent* TraceComp = Pair.Value.SpawnedActor->FindComponentByClass<UEVWeaponTraceComponent>())
		{
			TraceComp->SetDebug(true);
			UE_LOG(LogTemp, Log, TEXT("Combat.ShowDebug: Enabled debug on '%s'."), *Pair.Key.ToString());
		}
	}
}

// Combat.HideDebug

void UEVCheatManager::HandleHideCombatDebug(const TArray<FString>& Args)
{
	ACharacter* Character = GetPlayerController() ? GetPlayerController()->GetCharacter() : nullptr;
	if (!Character) return;

	const UEVEquipmentManagerComponent* EquipmentManager = Character->FindComponentByClass<UEVEquipmentManagerComponent>();
	if (!EquipmentManager) return;

	for (const auto& Pair : EquipmentManager->GetAllEquippedItems())
	{
		if (!IsValid(Pair.Value.SpawnedActor)) continue;
		if (UEVWeaponTraceComponent* TraceComp = Pair.Value.SpawnedActor->FindComponentByClass<UEVWeaponTraceComponent>())
		{
			TraceComp->SetDebug(false);
			UE_LOG(LogTemp, Log, TEXT("Combat.HideDebug: Disabled debug on '%s'."), *Pair.Key.ToString());
		}
	}
}

