// 


#include "AbilitySystem/AttributeSets/EVCombatAttributes.h"

void UEVCombatAttributes::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetMaxChainAttacksAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 1.f, 3.f);
	}
}
