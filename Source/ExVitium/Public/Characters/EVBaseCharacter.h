// 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "AbilitySystem/AttributeSets/EVAttributeSet.h"
#include "EVBaseCharacter.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
class UAbilitySystemComponent;

UCLASS(Abstract)
class EXVITIUM_API AEVBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AEVBaseCharacter();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
protected:
	void GrantDefaultAbilities();
	void ApplyDefaultEffects();
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "EV|GAS|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "EV|GAS|Attributes")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;
};
