// 

#pragma once

#include "CoreMinimal.h"
#include "EVEquipmentDefinition.h"
#include "EVWeaponDefinition.generated.h"

class UAnimMontage;

/**
 * Weapon item definition. Contains weapon-specific properties: type, attack montages, damage stats.
 * Inherits spawn/attach/mesh/equip data from UEVEquipmentDefinition.
 */
UCLASS()
class EXVITIUM_API UEVWeaponDefinition : public UEVEquipmentDefinition
{
	GENERATED_BODY()
	
public:
	UEVWeaponDefinition();
	
	// Type
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Tags|Category")
	FGameplayTag WeaponType;
	
	// Combat Animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Animations")
	TArray<TSoftObjectPtr<UAnimMontage>> LightAttackMontages;
	
	// Stats
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Stats")
	float BaseDamage = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Stats")
	float LightAttackStaminaCost = 0.f;

	virtual void GetAssetsToPreload(TArray<FSoftObjectPath>& OutPaths) const override;
	virtual void CollectGrantedTags(FGameplayTagContainer& OutTags) const override;
};
