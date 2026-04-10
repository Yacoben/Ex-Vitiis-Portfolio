// 


#include "AbilitySystem/Abilities/EVGA_Equip.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Characters/EVPlayerCharacter.h"
#include "Components/EVEquipmentManagerComponent.h"
#include "GameplayTags/EVGameplayTags.h"
#include "PDA/EVEquipmentDefinition.h"

UEVGA_Equip::UEVGA_Equip()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = EVTags::Events::Equipment::Equip;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
	
	// Tag defining Gameplay Ability
	SetAssetTags(FGameplayTagContainer(EVTags::Abilities::Equipment::Equip));
	
	// Blocked abilities with tag
	BlockAbilitiesWithTag.AddTag(EVTags::Abilities::Equipment::Equip);
	BlockAbilitiesWithTag.AddTag(EVTags::Abilities::Equipment::Unequip);
	BlockAbilitiesWithTag.AddTag(EVTags::Abilities::Combat::LightAttack);
	
	// Tags given on activation ability
	ActivationOwnedTags.AddTag(EVTags::State::Equipment::Equipping);
}

void UEVGA_Equip::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!TriggerEventData || TriggerEventData->TargetTags.Num() == 0)
	{
		FinishAbility(true);
		return;
	}

	// --- Extract data from event ---
	CachedDefinition = Cast<UEVEquipmentDefinition>(TriggerEventData->OptionalObject);
	if (!CachedDefinition)
	{
		FinishAbility(true);
		return;
	}

	CachedSlot = TriggerEventData->TargetTags.GetByIndex(0); // cached equipment slot
	bActorSpawned = false;
	bActorDestroyed = false;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		FinishAbility(true);
		return;
	}

	CachedEquipmentManager = AvatarActor->FindComponentByClass<UEVEquipmentManagerComponent>();
	if (!CachedEquipmentManager)
	{
		FinishAbility(true);
		return;
	}

	// --- Determine target meshes ---
	CachedTargetMesh = nullptr;
	CachedShadowMesh = nullptr;
	
	if (const AEVPlayerCharacter* PlayerCharacter = Cast<AEVPlayerCharacter>(AvatarActor))
	{
		CachedTargetMesh = PlayerCharacter->GetFirstPersonMesh();
		CachedShadowMesh = PlayerCharacter->GetMesh();
	}
	else if (const ACharacter* Character = Cast<ACharacter>(AvatarActor))
	{
		CachedTargetMesh = Character->GetMesh();
	}

	// --- Swap flow: if slot occupied, unequip old item first ---
	if (CachedEquipmentManager->IsSlotOccupied(CachedSlot))
	{
		const FEVEquippedItemEntry* OldEntry = CachedEquipmentManager->GetEquippedItem(CachedSlot);

		UAnimMontage* OldUnequipMontage = nullptr;
		if (OldEntry && OldEntry->Definition && !OldEntry->Definition->UnequipMontage.IsNull())
		{
			OldUnequipMontage = OldEntry->Definition->UnequipMontage.LoadSynchronous();
		}

		if (OldUnequipMontage)
		{
			bActorDestroyed = false;
			
			// Play old item's unequip montage
			UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this, NAME_None, OldUnequipMontage);
			MontageTask->OnCompleted.AddDynamic(this, &UEVGA_Equip::OnUnequipMontageEnded);
			MontageTask->OnBlendOut.AddDynamic(this, &UEVGA_Equip::OnUnequipMontageEnded);
			MontageTask->OnInterrupted.AddDynamic(this, &UEVGA_Equip::OnUnequipMontageEnded);
			MontageTask->OnCancelled.AddDynamic(this, &UEVGA_Equip::OnUnequipMontageEnded);
			MontageTask->ReadyForActivation();

			// Wait for despawn notify (AnimNotify in unequip montage)
			UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this, EVTags::Events::Equipment::MontageNotify, nullptr, true);
			EventTask->EventReceived.AddDynamic(this, &UEVGA_Equip::OnDespawnNotifyReceived);
			EventTask->ReadyForActivation();
			return;
		}

		// No montage on old item — unequip immediately
		CachedEquipmentManager->UnequipItem(CachedSlot, GetAbilitySystemComponentFromActorInfo());
	}

	PerformEquip();
}

void UEVGA_Equip::OnDespawnNotifyReceived(FGameplayEventData Payload)
{
	if (!bActorDestroyed)
	{
		CachedEquipmentManager->DestroyEquippedActor(CachedSlot);
		bActorDestroyed = true;
	}
}

void UEVGA_Equip::OnUnequipMontageEnded()
{
	// Ensure actor is destroyed even if notify didn't fire (interrupted montage)
	if (!bActorDestroyed)
	{
		CachedEquipmentManager->DestroyEquippedActor(CachedSlot);
		bActorDestroyed = true;
	}

	// Full cleanup of old item (GE + tags + entry)
	CachedEquipmentManager->UnequipItem(CachedSlot, GetAbilitySystemComponentFromActorInfo());
	
	// Now equip new item
	PerformEquip();
}

void UEVGA_Equip::PerformEquip()
{
	bActorSpawned = false;
	
	// Data phase: register item, apply GE + tags (no spawn yet)
	const bool bResult = CachedEquipmentManager->EquipItem(
		CachedDefinition, CachedSlot, GetAbilitySystemComponentFromActorInfo());

	if (!bResult)
	{
		FinishAbility(true);
		return;
	}

	// Check if item has equip montage
	UAnimMontage* EquipMontage = nullptr;
	if (CachedDefinition && !CachedDefinition->EquipMontage.IsNull())
	{
		EquipMontage = CachedDefinition->EquipMontage.LoadSynchronous();
	}

	if (EquipMontage)
	{
		// Play equip montage
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, EquipMontage);
		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnEquipMontageEnded);
		MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnEquipMontageEnded);
		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnEquipMontageEnded);
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnEquipMontageEnded);
		MontageTask->ReadyForActivation();

		// Wait for spawn notify (AnimNotify in equip montage)
		UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, EVTags::Events::Equipment::MontageNotify, nullptr, true);
		EventTask->EventReceived.AddDynamic(this, &ThisClass::OnSpawnNotifyReceived);
		EventTask->ReadyForActivation();
		return;
	}

	// No montage (stat-only item OR item with SpawnableActorClass but no montage) — spawn immediately
	CachedEquipmentManager->SpawnEquippedActor(CachedSlot, CachedTargetMesh, CachedShadowMesh);
	bActorSpawned = true;
	FinishAbility(false);
}

void UEVGA_Equip::OnSpawnNotifyReceived(FGameplayEventData Payload)
{
	if (!bActorSpawned)
	{
		CachedEquipmentManager->SpawnEquippedActor(CachedSlot, CachedTargetMesh, CachedShadowMesh);
		bActorSpawned = true;
	}
}

void UEVGA_Equip::OnEquipMontageEnded()
{
	// Ensure actor is spawned even if notify didn't fire (interrupted montage)
	if (!bActorSpawned)
	{
		CachedEquipmentManager->SpawnEquippedActor(CachedSlot, CachedTargetMesh, CachedShadowMesh);
		bActorSpawned = true;
	}
	
	FinishAbility(false);
}

void UEVGA_Equip::FinishAbility(const bool bWasCancelled)
{
	CachedDefinition = nullptr;
	CachedSlot = FGameplayTag();
	CachedTargetMesh = nullptr;
	CachedShadowMesh = nullptr;
	CachedEquipmentManager = nullptr;
	bActorSpawned = false;
	bActorDestroyed = false;
	
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, bWasCancelled);
}
