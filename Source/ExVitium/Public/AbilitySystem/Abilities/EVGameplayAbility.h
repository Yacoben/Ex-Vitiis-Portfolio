// 

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "EVGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class EXVITIUM_API UEVGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug")
	bool bDebug;
};
