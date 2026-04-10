// 


#include "AbilitySystem/Abilities/EVGA_LightAttack.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/AttributeSets/EVCombatAttributes.h"
#include "AbilitySystem/Tasks/EVAbilityTask_WeaponTrace.h"
#include "Components/EVEquipmentManagerComponent.h"
#include "GameplayTags/EVGameplayTags.h"
#include "PDA/EVWeaponDefinition.h"

UEVGA_LightAttack::UEVGA_LightAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	
	// Tag defining Gameplay Ability
	SetAssetTags(FGameplayTagContainer(EVTags::Abilities::Combat::LightAttack));
	
	// Required tags
	ActivationRequiredTags.AddTag(EVTags::State::Equipped::MainHand);
	
	// Blocked abilities with tag
	BlockAbilitiesWithTag.AddTag(EVTags::Abilities::Combat::LightAttack);
	BlockAbilitiesWithTag.AddTag(EVTags::Abilities::Equipment::Equip);
	BlockAbilitiesWithTag.AddTag(EVTags::Abilities::Equipment::Unequip);
	
	// Tags given on activation ability
	ActivationOwnedTags.AddTag(EVTags::State::Combat::Attacking);
	
}

void UEVGA_LightAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// Check if avatar has equipment managercomponent
	const UEVEquipmentManagerComponent* AvatarEquipment = ActorInfo->AvatarActor->FindComponentByClass<UEVEquipmentManagerComponent>();
	if (!AvatarEquipment)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// Get equipped item in MainHand and check if it is valid
	const FEVEquippedItemEntry* MainHandEntry = AvatarEquipment->GetEquippedItem(EVTags::EquipmentSlot::MainHand);
	if (!MainHandEntry)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// Get weapon definition from equipped item and check if it is valid
	const UEVWeaponDefinition* EquippedWeaponDefinition = Cast<UEVWeaponDefinition>(MainHandEntry->Definition);
	if (!EquippedWeaponDefinition)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// Get Light Attack Montages from equipped weapon and check it it is valid
	AttackMontages = EquippedWeaponDefinition->LightAttackMontages;
	if (AttackMontages.IsEmpty())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// Check if ability can be committed (costs, cooldown, etc.) and commit it. If it can't be committed end ability
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// Initialize combo state
	ComboIndex = 0;
	
	// Listen for chain attack EVENTS (once for the entire ability lifetime)
	// Chain window OPENED
	UAbilityTask_WaitGameplayEvent* ChainWindowOpened = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EVTags::Events::Combat::ChainAttackWindowOpened, nullptr, false);
	ChainWindowOpened->EventReceived.AddDynamic(this, &ThisClass::OnChainWindowOpened);
	ChainWindowOpened->ReadyForActivation();
	// Chain window CLOSED
	UAbilityTask_WaitGameplayEvent* ChainWindowClosed = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EVTags::Events::Combat::ChainAttackWindowClosed, nullptr, false);
	ChainWindowClosed->EventReceived.AddDynamic(this, &ThisClass::OnChainWindowClosed);
	ChainWindowClosed->ReadyForActivation();
	// Chain input RECEIVED
	UAbilityTask_WaitGameplayEvent* ChainInputReceived = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EVTags::Events::Combat::ChainAttack, nullptr, false);
	ChainInputReceived->EventReceived.AddDynamic(this, &ThisClass::OnChainInputReceived);
	ChainInputReceived->ReadyForActivation();	
	
	UEVAbilityTask_WeaponTrace* WeaponTraceTask = UEVAbilityTask_WeaponTrace::CreateWeaponTraceTask(this, EVTags::EquipmentSlot::MainHand);
	WeaponTraceTask->OnWeaponTraceTaskHit.AddDynamic(this, &ThisClass::OnWeaponHit);
	WeaponTraceTask->ReadyForActivation();
	
	// Play Attack Montage
	PlayAttackMontage(ComboIndex);
}

void UEVGA_LightAttack::PlayAttackMontage(int32 MontageIndex)
{
	// Check if attack montages of montage index is valid
	if (!AttackMontages.IsValidIndex(MontageIndex))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	// Get attack montage to play from AttackMontages array and check if it is valid
	UAnimMontage* AttackMontageToPlay = AttackMontages[MontageIndex].LoadSynchronous(); // Instant — assets preloaded by EquipmentManager on equip
	if (!AttackMontageToPlay)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// Clear old montage task delegates so the interrupt from chain transition doesn't end the ability
	if (CurrentMontageTask)
	{
		CurrentMontageTask->OnCompleted.Clear();
		CurrentMontageTask->OnBlendOut.Clear();
		CurrentMontageTask->OnInterrupted.Clear();
		CurrentMontageTask->OnCancelled.Clear();
	}
	
	// Bind all exec pins from montage to methods Completed, Cancelled
	CurrentMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontageToPlay);
	CurrentMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnComboMontageCompleted);
	CurrentMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnComboMontageCancelled);
	CurrentMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnComboMontageCancelled);
	CurrentMontageTask->ReadyForActivation();
}

void UEVGA_LightAttack::OnComboMontageCompleted()
{
	if (!IsActive()) return;
	ResetCombo();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UEVGA_LightAttack::OnComboMontageCancelled()
{
	if (!IsActive()) return;
	ResetCombo();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}

void UEVGA_LightAttack::OnChainWindowOpened(FGameplayEventData Payload) // When anim notify chain attack window state start
{
	bChainAttackWindowOpened = true;
}

void UEVGA_LightAttack::OnChainWindowClosed(FGameplayEventData Payload) // When anim notify chain attack window state ends
{
	bChainAttackWindowOpened = false;
}

void UEVGA_LightAttack::OnChainInputReceived(FGameplayEventData Payload) // When player inputs attack during chain attack window (because everything is on 1 instance of GA)
{
	if (!bChainAttackWindowOpened)  return; // input outside of the window -> ignore
	if (ComboIndex + 1 >= GetEffectiveMaxChain()) return; // already at max combo -> ignore
	
	bChainAttackWindowOpened = false; // Reset — next attack must wait for its own chain window
	ComboIndex++;
	PlayAttackMontage(ComboIndex);
}

void UEVGA_LightAttack::ResetCombo()
{
	ComboIndex = 0;
	bChainAttackWindowOpened = false;
}

void UEVGA_LightAttack::OnWeaponHit(FGameplayEventData EventData)
{
	// TODO: Apply damage via GA_ApplyDamage or GameplayEffect
	
	if (!EventData.TargetData.IsValid(0)) return;
    
	const FHitResult* HitResult = EventData.TargetData.Get(0)->GetHitResult();
	if (!HitResult) return;
    
	UE_LOG(LogTemp, Log, TEXT("LightAttack: Weapon hit actor %s at %s location"), *HitResult->GetActor()->GetName(), *HitResult->Location.ToString());
}

int32 UEVGA_LightAttack::GetEffectiveMaxChain() const
{
	const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!IsValid(ASC)) return AttackMontages.Num(); // if ASC is not valid return length of attack montages array
	
	const float MaxChainAttribute = ASC->GetNumericAttribute(UEVCombatAttributes::GetMaxChainAttacksAttribute()); // Get MaxChainAttribute
	return FMath::Min(static_cast<int32>(MaxChainAttribute), AttackMontages.Num()); // returns lower value between MaxChainAttribute value and attack montages num
}

