// 


#include "AbilitySystem/Tasks/EVAbilityTask_WeaponTrace.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/EVEquipmentManagerComponent.h"
#include "Components/EVWeaponTraceComponent.h"

UEVAbilityTask_WeaponTrace* UEVAbilityTask_WeaponTrace::CreateWeaponTraceTask(UGameplayAbility* OwningAbility, FGameplayTag InWeaponSlot)
{
	UEVAbilityTask_WeaponTrace* Task = NewAbilityTask<UEVAbilityTask_WeaponTrace>(OwningAbility);
	Task->WeaponSlot = InWeaponSlot;
	return Task;
}

void UEVAbilityTask_WeaponTrace::Activate()
{
	Super::Activate();

	const AActor* AvatarActor = GetAvatarActor();
	if (!IsValid(AvatarActor)) return;
	
	// Get WeaponTraceComponent through EquipmentManager → EquippedItem → SpawnedActor
	const UEVEquipmentManagerComponent* EquipmentManager = AvatarActor->FindComponentByClass<UEVEquipmentManagerComponent>();
	if (!IsValid(EquipmentManager)) return;
	
	const FEVEquippedItemEntry* EquippedItem = EquipmentManager->GetEquippedItem(WeaponSlot);
	if (!EquippedItem || !IsValid(EquippedItem->SpawnedActor)) return;

	UEVWeaponTraceComponent* WeaponTraceComponent = EquippedItem->SpawnedActor->FindComponentByClass<UEVWeaponTraceComponent>();
	if (!IsValid(WeaponTraceComponent)) return;
	
	CachedWeaponTraceComponent = WeaponTraceComponent;
	CachedWeaponTraceComponent->OnWeaponTraceHit.AddDynamic(this, &ThisClass::HandleOnWeaponTraceHit);

	// Listen for anim notify gameplay events to control trace timing
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC)
	{
		TraceStartHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(EVTags::Events::Combat::WeaponCollisionStart).AddUObject(this, &ThisClass::OnTraceStart);
		TraceEndHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(EVTags::Events::Combat::WeaponCollisionEnd).AddUObject(this, &ThisClass::OnTraceEnd);
	}
}

void UEVAbilityTask_WeaponTrace::OnDestroy(bool bInOwnerFinished)
{
	if (CachedWeaponTraceComponent)
	{
		CachedWeaponTraceComponent->EndTrace();
		CachedWeaponTraceComponent->OnWeaponTraceHit.RemoveDynamic(this, &ThisClass::HandleOnWeaponTraceHit);
		CachedWeaponTraceComponent = nullptr;
	}

	// Unbind from ASC gameplay events
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC)
	{
		ASC->GenericGameplayEventCallbacks.FindOrAdd(EVTags::Events::Combat::WeaponCollisionStart).Remove(TraceStartHandle);
		ASC->GenericGameplayEventCallbacks.FindOrAdd(EVTags::Events::Combat::WeaponCollisionEnd).Remove(TraceEndHandle);
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

void UEVAbilityTask_WeaponTrace::OnTraceStart(const FGameplayEventData* EventData)
{
	if (CachedWeaponTraceComponent)
	{
		CachedWeaponTraceComponent->StartTrace();
	}
}

void UEVAbilityTask_WeaponTrace::OnTraceEnd(const FGameplayEventData* EventData)
{
	if (CachedWeaponTraceComponent)
	{
		CachedWeaponTraceComponent->EndTrace();
	}
}

void UEVAbilityTask_WeaponTrace::HandleOnWeaponTraceHit(const FHitResult& HitResult)
{
	FGameplayEventData EventData;
	EventData.EventTag = EVTags::Events::Combat::WeaponHit;
	EventData.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(HitResult);
	
	OnWeaponTraceTaskHit.Broadcast(EventData);
}
