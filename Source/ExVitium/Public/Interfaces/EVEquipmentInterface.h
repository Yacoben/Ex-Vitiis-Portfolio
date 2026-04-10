// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "EVEquipmentInterface.generated.h"

class UEVItemDefinition;

UINTERFACE(MinimalAPI)
class UEVEquipmentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class EXVITIUM_API IEVEquipmentInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Equipment")
	bool TryEquip(UEVItemDefinition* ItemDefinition, FGameplayTag EquipmentSlot);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Equipment")
	bool TryUnequip(FGameplayTag EquipmentSlot);
};
