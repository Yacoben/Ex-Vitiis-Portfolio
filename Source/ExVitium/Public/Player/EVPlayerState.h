// 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "EVPlayerState.generated.h"


class UEVCombatAttributes;
class UEVAbilitySystemComponent;
class UEVAttributeSet;
class UEVVitalAttributeSet;

/**
 * 
 */
UCLASS()
class EXVITIUM_API AEVPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AEVPlayerState();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Returns first AttributeSet of given type e.g. GetAttributeSet<UEVVitalAttributeSet>()
	template<typename T>
	T* GetAttributeSet() const
	{
		for (UEVAttributeSet* AS : AttributeSets)
		{
			if (T* Found = Cast<T>(AS))
			{
				return Found;
			}
		}
		return nullptr;
	}
	
private:
	UPROPERTY(VisibleAnywhere, Category = "ExVitium|ASC")
	TObjectPtr<UEVAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UEVVitalAttributeSet> VitalAttributeSet;
	
	UPROPERTY()
	TObjectPtr<UEVCombatAttributes> CombatAttributeSet;
	
	UPROPERTY()
	TArray<TObjectPtr<UEVAttributeSet>> AttributeSets;
};
