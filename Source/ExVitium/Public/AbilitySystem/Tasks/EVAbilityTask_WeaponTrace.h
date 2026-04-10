// 

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameplayTags/EVGameplayTags.h"
#include "EVAbilityTask_WeaponTrace.generated.h"


class UEVWeaponTraceComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponTraceTaskHit, FGameplayEventData, EventData);

UCLASS()
class EXVITIUM_API UEVAbilityTask_WeaponTrace : public UAbilityTask
{
	GENERATED_BODY()

public:
	virtual void OnDestroy(bool bInOwnerFinished) override;
	
	UPROPERTY(BlueprintAssignable)
	FOnWeaponTraceTaskHit OnWeaponTraceTaskHit;
	
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "Weapon Trace", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UEVAbilityTask_WeaponTrace* CreateWeaponTraceTask(UGameplayAbility* OwningAbility, FGameplayTag WeaponSlot);

protected:
	virtual void Activate() override;

private:
	UFUNCTION()
	void HandleOnWeaponTraceHit(const FHitResult& HitResult);

	void OnTraceStart(const FGameplayEventData* EventData);
	void OnTraceEnd(const FGameplayEventData* EventData);

	UPROPERTY()
	TObjectPtr<UEVWeaponTraceComponent> CachedWeaponTraceComponent = nullptr;
	
	FGameplayTag WeaponSlot;

	FDelegateHandle TraceStartHandle;
	FDelegateHandle TraceEndHandle;
};
