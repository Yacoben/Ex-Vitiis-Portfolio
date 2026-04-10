// 


#include "PDA/EVWeaponDefinition.h"

#include "GameplayTags/EVGameplayTags.h"

UEVWeaponDefinition::UEVWeaponDefinition()
{
	ItemCategory = EVTags::ItemCategory::Equipment::Weapon;
	MaxStackSize = 1;
}

void UEVWeaponDefinition::GetAssetsToPreload(TArray<FSoftObjectPath>& OutPaths) const
{
	Super::GetAssetsToPreload(OutPaths);

	for (const TSoftObjectPtr<UAnimMontage>& Montage : LightAttackMontages)
	{
		if (!Montage.IsNull())
		{
			OutPaths.Add(Montage.ToSoftObjectPath());
		}
	}
}

void UEVWeaponDefinition::CollectGrantedTags(FGameplayTagContainer& OutTags) const
{
	Super::CollectGrantedTags(OutTags);

	if (WeaponType.IsValid())
	{
		OutTags.AddTag(WeaponType);
	}
}

