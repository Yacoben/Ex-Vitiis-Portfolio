// 

#include "Characters/EVBaseCharacter.h"
#include "AbilitySystem/EVAbilitySystemComponent.h"
#include "GameplayEffect.h"


AEVBaseCharacter::AEVBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
}

UAbilitySystemComponent* AEVBaseCharacter::GetAbilitySystemComponent() const
{
	checkf(false, TEXT("GetAbilitySystemComponent() not overriden in %s"), *GetClass()->GetName());
	return nullptr;
}

void AEVBaseCharacter::GrantDefaultAbilities()
{
	if (!IsValid(GetAbilitySystemComponent())) return;
	
	for (const TSubclassOf<UGameplayAbility>& DefaultAbility : DefaultAbilities)
	{
		if (DefaultAbility)
		{
			if (GetAbilitySystemComponent()->FindAbilitySpecFromClass(DefaultAbility)) // check if ability already exist on ASC
			{
				continue;
			}
			GetAbilitySystemComponent()->GiveAbility(FGameplayAbilitySpec(DefaultAbility));
		}
	}
}

void AEVBaseCharacter::ApplyDefaultEffects()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!IsValid(ASC)) return;

	for (const TSubclassOf<UGameplayEffect>& EffectClass : DefaultEffects)
	{
		if (!EffectClass) continue;

		// Skip if this effect is already active on ASC (preserves upgraded levels across respawns)
		FGameplayEffectQuery Query;
		Query.EffectDefinition = EffectClass;
		if (ASC->GetActiveEffects(Query).Num() > 0) continue;

		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		Context.AddSourceObject(this);

		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(EffectClass, 1.f, Context);
		if (Spec.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}
}

