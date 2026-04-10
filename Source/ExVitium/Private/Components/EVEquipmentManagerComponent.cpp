// 


#include "Components/EVEquipmentManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/World.h"
#include "GameplayTags/EVGameplayTags.h"
#include "Interfaces/EVEquipmentInitInterface.h"
#include "PDA/EVEquipmentDefinition.h"


UEVEquipmentManagerComponent::UEVEquipmentManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FGameplayTag UEVEquipmentManagerComponent::GetEquippedStateTag(const FGameplayTag& SlotTag)
{
	static const TMap<FGameplayTag, FGameplayTag> SlotToStateMap = {
		{ EVTags::EquipmentSlot::MainHand,  EVTags::State::Equipped::MainHand },
		{ EVTags::EquipmentSlot::OffHand,   EVTags::State::Equipped::OffHand },
		{ EVTags::EquipmentSlot::Head,      EVTags::State::Equipped::Head },
		{ EVTags::EquipmentSlot::Chest,     EVTags::State::Equipped::Chest },
		{ EVTags::EquipmentSlot::Hands,     EVTags::State::Equipped::Hands },
		{ EVTags::EquipmentSlot::Feet,      EVTags::State::Equipped::Feet },
		{ EVTags::EquipmentSlot::Ring,      EVTags::State::Equipped::Ring },
		{ EVTags::EquipmentSlot::Necklace,  EVTags::State::Equipped::Necklace },
	};

	if (const FGameplayTag* Found = SlotToStateMap.Find(SlotTag))
	{
		return *Found;
	}
	return FGameplayTag();
}

bool UEVEquipmentManagerComponent::EquipItem(
	const UEVEquipmentDefinition* Definition,
	const FGameplayTag Slot,
	UAbilitySystemComponent* ASC)
{
	if (!Definition) return false;
	if (!Slot.IsValid()) return false;

	// Validate that this item is allowed in the requested slot
	if (!Definition->AllowedSlots.HasTag(Slot)) return false;

	// If slot is already occupied, unequip old item first (safety net)
	if (IsSlotOccupied(Slot))
	{
		UnequipItem(Slot, ASC);
	}

	FEVEquippedItemEntry Entry;
	Entry.Definition = Definition;

	// --- Apply Gameplay Effect (stats, bonuses) ---
	if (ASC && Definition->OnEquipEffect)
	{
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddSourceObject(GetOwner());

		const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
			Definition->OnEquipEffect, 1, EffectContext);

		if (SpecHandle.IsValid())
		{
			Entry.ActiveEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	// --- Grant tags to ASC ---
	FGameplayTagContainer AllGrantedTags;
	Definition->CollectGrantedTags(AllGrantedTags);
	
	if (ASC && !AllGrantedTags.IsEmpty())
	{
		for (const FGameplayTag& Tag : AllGrantedTags)
		{
			ASC->AddLooseGameplayTag(Tag);
		}
		Entry.AppliedTags = AllGrantedTags;
	}

	// --- Mark slot as occupied on ASC ---
	if (ASC)
	{
		ASC->AddLooseGameplayTag(Slot);

		// Add state tag (e.g. EVTags.State.Equipped.MainHand)
		const FGameplayTag StateTag = GetEquippedStateTag(Slot);
		if (StateTag.IsValid())
		{
			ASC->AddLooseGameplayTag(StateTag);
		}
	}

	// --- Async preload assets (montages, meshes) so they're cached for instant access ---
	TArray<FSoftObjectPath> AssetsToPreload;
	Definition->GetAssetsToPreload(AssetsToPreload);
	if (AssetsToPreload.Num() > 0)
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		Entry.PreloadHandle = StreamableManager.RequestAsyncLoad(AssetsToPreload);
	}

	// --- Grant abilities from equipment definition ---
	if (ASC)
	{
		for (const TSubclassOf<UGameplayAbility>& AbilityClass : Definition->GrantedAbilities)
		{
			if (!AbilityClass) continue;
			const FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(FGameplayAbilitySpec(AbilityClass));
			Entry.GrantedAbilityHandles.Add(Handle);
		}
	}

	EquippedItems.Add(Slot, Entry);
	return true;
}

bool UEVEquipmentManagerComponent::SpawnEquippedActor(
	const FGameplayTag Slot,
	USkeletalMeshComponent* TargetMesh,
	USkeletalMeshComponent* ShadowMesh)
{
	FEVEquippedItemEntry* Entry = EquippedItems.Find(Slot);
	if (!Entry || !Entry->Definition) return false;

	// Already spawned or nothing to spawn
	if (Entry->SpawnedActor && IsValid(Entry->SpawnedActor)) return false;
	if (!Entry->Definition->SpawnableActorClass) return false;
	if (!TargetMesh) return false;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		Entry->Definition->SpawnableActorClass,
		FTransform::Identity,
		SpawnParams);

	if (!SpawnedActor) return false;

	// Initialize via interface (sets mesh, weapon data, etc.)
	if (SpawnedActor->Implements<UEVEquipmentInitInterface>())
	{
		IEVEquipmentInitInterface::Execute_InitializeFromDefinition(SpawnedActor, Entry->Definition);
	}

	// Attach to target mesh
	SpawnedActor->AttachToComponent(
		TargetMesh,
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		Entry->Definition->AttachSocketName);

	// Attach shadow representation
	if (ShadowMesh && SpawnedActor->Implements<UEVEquipmentInitInterface>())
	{
		IEVEquipmentInitInterface::Execute_AttachShadowMesh(SpawnedActor, ShadowMesh, Entry->Definition->AttachSocketName);
	}

	Entry->SpawnedActor = SpawnedActor;
	return true;
}

void UEVEquipmentManagerComponent::DestroyEquippedActor(const FGameplayTag Slot)
{
	FEVEquippedItemEntry* Entry = EquippedItems.Find(Slot);
	if (!Entry) return;

	if (Entry->SpawnedActor && IsValid(Entry->SpawnedActor))
	{
		Entry->SpawnedActor->Destroy();
		Entry->SpawnedActor = nullptr;
	}
}

bool UEVEquipmentManagerComponent::UnequipItem(const FGameplayTag Slot, UAbilitySystemComponent* ASC)
{
	FEVEquippedItemEntry* Entry = EquippedItems.Find(Slot);
	if (!Entry) return false;

	// --- Destroy spawned actor (if not already destroyed by notify) ---
	if (Entry->SpawnedActor && IsValid(Entry->SpawnedActor))
	{
		Entry->SpawnedActor->Destroy();
	}

	// --- Remove Gameplay Effect ---
	if (ASC && Entry->ActiveEffectHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(Entry->ActiveEffectHandle);
	}

	// --- Remove granted abilities ---
	if (ASC)
	{
		for (const FGameplayAbilitySpecHandle& Handle : Entry->GrantedAbilityHandles)
		{
			ASC->ClearAbility(Handle);
		}
	}

	// --- Remove granted tags ---
	if (ASC)
	{
		for (const FGameplayTag& Tag : Entry->AppliedTags)
		{
			ASC->RemoveLooseGameplayTag(Tag);
		}

		// Remove slot occupied tag
		ASC->RemoveLooseGameplayTag(Slot);

		// Remove state tag (e.g. EVTags.State.Equipped.MainHand)
		const FGameplayTag StateTag = GetEquippedStateTag(Slot);
		if (StateTag.IsValid())
		{
			ASC->RemoveLooseGameplayTag(StateTag);
		}
	}

	EquippedItems.Remove(Slot);
	return true;
}

bool UEVEquipmentManagerComponent::IsSlotOccupied(const FGameplayTag Slot) const
{
	return EquippedItems.Contains(Slot);
}

const FEVEquippedItemEntry* UEVEquipmentManagerComponent::GetEquippedItem(const FGameplayTag Slot) const
{
	return EquippedItems.Find(Slot);
}

float UEVEquipmentManagerComponent::GetWeightAllEquippedItems() const
{
	if (EquippedItems.Num() == 0) return 0.f;
	
	float Sum = 0.f;
	
	for (const auto& Pair : EquippedItems)
	{
		if (Pair.Value.Definition)
		{
			Sum += Pair.Value.Definition->Weight;
		}
	}
	
	return Sum;
}


