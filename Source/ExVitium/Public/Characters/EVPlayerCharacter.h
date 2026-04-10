// 

#pragma once

#include "CoreMinimal.h"
#include "EVBaseCharacter.h"
#include "Interfaces/EVEquipmentInterface.h"
#include "EVPlayerCharacter.generated.h"

class UEVFootstepComponent;
class UEVItemDefinition;
class UAttributeSet;
class USkeletalMeshComponent;
class UCameraComponent;
class UEVEquipmentManagerComponent;

UCLASS()
class EXVITIUM_API AEVPlayerCharacter : public AEVBaseCharacter, public IEVEquipmentInterface
{
	GENERATED_BODY()

public:
	AEVPlayerCharacter();

	USkeletalMeshComponent* GetFirstPersonMesh() const { return ArmsMesh; }
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	UEVEquipmentManagerComponent* GetEquipmentManager() const { return EquipmentManager; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual bool TryEquip_Implementation(UEVItemDefinition* ItemDefinition, FGameplayTag EquipmentSlot) override;
	virtual bool TryUnequip_Implementation(FGameplayTag EquipmentSlot) override;

protected:
	virtual void PossessedBy(AController* NewController) override;
	
private:
	void SetupConfig();
	void InitializeAbilitySystem();
	void InitializeAnimInstance() const;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ArmsMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="EV|Equipment", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEVEquipmentManagerComponent> EquipmentManager;
};
