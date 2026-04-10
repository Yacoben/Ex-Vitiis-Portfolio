// 


#include "PDA/EVEquipmentDefinition.h"

UEVEquipmentDefinition::UEVEquipmentDefinition()
{
	MaxStackSize = 1;
}

void UEVEquipmentDefinition::GetAssetsToPreload(TArray<FSoftObjectPath>& OutPaths) const
{
	if (!EquipMontage.IsNull())
	{
		OutPaths.Add(EquipMontage.ToSoftObjectPath());
	}
	if (!UnequipMontage.IsNull())
	{
		OutPaths.Add(UnequipMontage.ToSoftObjectPath());
	}
	if (!EquipmentMesh.IsNull())
	{
		OutPaths.Add(EquipmentMesh.ToSoftObjectPath());
	}
	if (!EquipmentSkeletalMesh.IsNull())
	{
		OutPaths.Add(EquipmentSkeletalMesh.ToSoftObjectPath());
	}
}

void UEVEquipmentDefinition::CollectGrantedTags(FGameplayTagContainer& OutTags) const
{
	OutTags.AppendTags(GrantedTags);
}

