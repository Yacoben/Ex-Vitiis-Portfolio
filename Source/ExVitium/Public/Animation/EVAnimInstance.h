//
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"
#include "EVAnimInstance.generated.h"

class UAbilitySystemComponent;

UENUM(BlueprintType)
enum class EEVPlayerAnimState : uint8
{
	Unarmed = 0,
	Sword,
	Longsword,
	Bow
};

UCLASS()
class EXVITIUM_API UEVAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);
	virtual void NativeUninitializeAnimation() override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "EV|Animation")
	EEVPlayerAnimState PlayerAnimState = EEVPlayerAnimState::Unarmed;
	
	UPROPERTY(EditDefaultsOnly, Category = "EV|Animation")
	TMap<FGameplayTag, EEVPlayerAnimState> WeaponAnimStateMap;

private:
	void OnWeaponTypeTagChanged(const FGameplayTag Tag, int32 NewCount);

	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;
	
	FDelegateHandle WeaponTagDelegateHandle;
};
