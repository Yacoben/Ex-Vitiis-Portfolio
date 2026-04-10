// 


#include "Player/EVPlayerState.h"
#include "AbilitySystem/EVAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/EVCombatAttributes.h"
#include "AbilitySystem/AttributeSets/EVVitalAttributeSet.h"

AEVPlayerState::AEVPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UEVAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	
	VitalAttributeSet = CreateDefaultSubobject<UEVVitalAttributeSet>(TEXT("VitalAttributeSet"));
	AttributeSets.Add(VitalAttributeSet);
	
	CombatAttributeSet = CreateDefaultSubobject<UEVCombatAttributes>(TEXT("CombatAttributeSet"));
	AttributeSets.Add(CombatAttributeSet);
}

UAbilitySystemComponent* AEVPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

