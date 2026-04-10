// 


#include "Characters/EVEnemyBaseCharacter.h"

#include "AbilitySystem/EVAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/EVVitalAttributeSet.h"


AEVEnemyBaseCharacter::AEVEnemyBaseCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UEVAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	
	VitalAttributeSet = CreateDefaultSubobject<UEVVitalAttributeSet>(TEXT("VitalAttributeSet"));
	AttributeSets.Add(VitalAttributeSet);
}

void AEVEnemyBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		GrantDefaultAbilities();
		ApplyDefaultEffects();
	}
}

UAbilitySystemComponent* AEVEnemyBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


