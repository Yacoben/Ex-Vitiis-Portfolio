// 

#pragma once

#include "CoreMinimal.h"
#include "HAL/IConsoleManager.h"
#include "GameFramework/CheatManager.h"
#include "EVCheatManager.generated.h"

class UEVEquipmentDefinition;

/**
 * Custom cheat manager using dot notation Equipment.Equip, Equipment.Unequip, etc.
 * Type "Equipment." to see all available commands.
 */
UCLASS()
class EXVITIUM_API UEVCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
public:
	UEVCheatManager();
	virtual void BeginDestroy() override;

private:
	void RegisterConsoleCommands();
	void UnregisterConsoleCommands();
	
	void HandleEquip(const TArray<FString>& Args);
	void HandleUnequip(const TArray<FString>& Args);
	void HandlePrintEquippedItems(const TArray<FString>& Args);
	void HandleSetMaxChainAttacksLevel(const TArray<FString>& Args);
	void HandleShowCombatDebug(const TArray<FString>& Args);
	void HandleHideCombatDebug(const TArray<FString>& Args);

	UPROPERTY()
	TArray<TObjectPtr<UObject>> KeepAliveRefs;
	
	TArray<IConsoleObject*> RegisteredCommands;
};
