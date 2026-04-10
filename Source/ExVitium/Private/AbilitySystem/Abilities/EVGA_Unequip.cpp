// 


#include "AbilitySystem/Abilities/EVGA_Unequip.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Components/EVEquipmentManagerComponent.h"
#include "GameplayTags/EVGameplayTags.h"
#include "PDA/EVEquipmentDefinition.h"

UEVGA_Unequip::UEVGA_Unequip()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = EVTags::Events::Equipment::Unequip;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
	
	// Tag defining Gameplay Ability
	SetAssetTags(FGameplayTagContainer(EVTags::Abilities::Equipment::Unequip));
	
	// Blocked abilities with tag
	BlockAbilitiesWithTag.AddTag(EVTags::Abilities::Equipment::Equip);
	BlockAbilitiesWithTag.AddTag(EVTags::Abilities::Equipment::Unequip);
	BlockAbilitiesWithTag.AddTag(EVTags::Abilities::Combat::LightAttack);
	
	// Tags given on activation ability
	ActivationOwnedTags.AddTag(EVTags::State::Equipment::Unequipping);
}

void UEVGA_Unequip::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
									const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!TriggerEventData || TriggerEventData->TargetTags.Num() == 0)
	{
		FinishAbility(true);
		return;
	}

	CachedSlot = TriggerEventData->TargetTags.GetByIndex(0);
	bActorDestroyed = false;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		FinishAbility(true);
		return;
	}

	CachedEquipmentManager = AvatarActor->FindComponentByClass<UEVEquipmentManagerComponent>();
	if (!CachedEquipmentManager || !CachedEquipmentManager->IsSlotOccupied(CachedSlot))
	{
		FinishAbility(true);
		return;
	}

	// --- Try to play unequip montage ---
	const FEVEquippedItemEntry* Entry = CachedEquipmentManager->GetEquippedItem(CachedSlot);
	
	UAnimMontage* UnequipMontage = nullptr;
	if (Entry && Entry->Definition && !Entry->Definition->UnequipMontage.IsNull())
	{
		UnequipMontage = Entry->Definition->UnequipMontage.LoadSynchronous();
	}

	if (UnequipMontage)
	{
		// Play unequip montage
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, UnequipMontage);
		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnUnequipMontageEnded);
		MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnUnequipMontageEnded);
		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnUnequipMontageEnded);
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnUnequipMontageEnded);
		MontageTask->ReadyForActivation();

		// Wait for despawn notify (AnimNotify in unequip montage)
		UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, EVTags::Events::Equipment::MontageNotify, nullptr, true);
		EventTask->EventReceived.AddDynamic(this, &ThisClass::OnDespawnNotifyReceived);
		EventTask->ReadyForActivation();
		return;
	}

	// No montage — unequip immediately
	PerformUnequip();
}

void UEVGA_Unequip::OnDespawnNotifyReceived(FGameplayEventData Payload)
{
	if (!bActorDestroyed)
	{
		CachedEquipmentManager->DestroyEquippedActor(CachedSlot);
		bActorDestroyed = true;
	}
}

void UEVGA_Unequip::OnUnequipMontageEnded()
{
	// Ensure actor is destroyed even if notify didn't fire (interrupted montage)
	if (!bActorDestroyed)
	{
		CachedEquipmentManager->DestroyEquippedActor(CachedSlot);
		bActorDestroyed = true;
	}

	PerformUnequip();
}

void UEVGA_Unequip::PerformUnequip()
{
	const bool bResult = CachedEquipmentManager->UnequipItem(CachedSlot, GetAbilitySystemComponentFromActorInfo());
	FinishAbility(!bResult);
}

void UEVGA_Unequip::FinishAbility(const bool bWasCancelled)
{
	CachedSlot = FGameplayTag();
	CachedEquipmentManager = nullptr;
	bActorDestroyed = false;
	
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, bWasCancelled);
}
