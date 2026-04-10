//
#include "Animation/EVAnimInstance.h"
#include "AbilitySystemComponent.h"
#include "GameplayTags/EVGameplayTags.h"

void UEVAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);
	CachedASC = ASC;

	WeaponTagDelegateHandle = ASC->RegisterGameplayTagEvent(
		EVTags::WeaponType::WeaponTypeRoot,
		EGameplayTagEventType::AnyCountChange
	).AddUObject(this, &UEVAnimInstance::OnWeaponTypeTagChanged);
}

void UEVAnimInstance::NativeUninitializeAnimation()
{
	if (CachedASC.IsValid() && WeaponTagDelegateHandle.IsValid())
	{
		CachedASC->RegisterGameplayTagEvent(
			EVTags::WeaponType::WeaponTypeRoot,
			EGameplayTagEventType::AnyCountChange
		).Remove(WeaponTagDelegateHandle);
	}

	Super::NativeUninitializeAnimation();
}

void UEVAnimInstance::OnWeaponTypeTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (!CachedASC.IsValid())
	{
		PlayerAnimState = EEVPlayerAnimState::Unarmed;
		return;
	}

	FGameplayTagContainer OwnedTags;
	CachedASC->GetOwnedGameplayTags(OwnedTags);

	for (const auto& [WeaponTag, AnimState] : WeaponAnimStateMap)
	{
		if (OwnedTags.HasTagExact(WeaponTag))
		{
			PlayerAnimState = AnimState;
			return;
		}
	}

	PlayerAnimState = EEVPlayerAnimState::Unarmed;
}
