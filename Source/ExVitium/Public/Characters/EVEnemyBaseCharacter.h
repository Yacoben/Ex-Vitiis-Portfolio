// 

#pragma once

#include "CoreMinimal.h"
#include "EVBaseCharacter.h"
#include "EVEnemyBaseCharacter.generated.h"

class UEVVitalAttributeSet;
class UEVAbilitySystemComponent;

UCLASS()
class EXVITIUM_API AEVEnemyBaseCharacter : public AEVBaseCharacter
{
	GENERATED_BODY()

public:
	AEVEnemyBaseCharacter();

protected:
	virtual void BeginPlay() override;
	
public:
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
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UEVAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UEVVitalAttributeSet> VitalAttributeSet;
	
	UPROPERTY()
	TArray<TObjectPtr<UEVAttributeSet>> AttributeSets;
};
