// 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "EVAttributeSet.h"
#include "EVVitalAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class EXVITIUM_API UEVVitalAttributeSet : public UEVAttributeSet
{
	GENERATED_BODY()
	
public:
	UEVVitalAttributeSet();
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	ATTRIBUTE_ACCESSORS(UEVVitalAttributeSet, MaxHealth)
	ATTRIBUTE_ACCESSORS(UEVVitalAttributeSet, Health)
	ATTRIBUTE_ACCESSORS(UEVVitalAttributeSet, MaxStamina)
	ATTRIBUTE_ACCESSORS(UEVVitalAttributeSet, Stamina)
	
private:
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxHealth;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Health;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxStamina;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Stamina;
	
};
