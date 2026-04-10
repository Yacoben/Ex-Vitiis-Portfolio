// 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "EVAttributeSet.h"
#include "EVCombatAttributes.generated.h"

/**
 * 
 */
UCLASS()
class EXVITIUM_API UEVCombatAttributes : public UEVAttributeSet
{
	GENERATED_BODY()
	
	public:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	ATTRIBUTE_ACCESSORS(UEVCombatAttributes, MaxChainAttacks)
	
private:
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxChainAttacks;
};
