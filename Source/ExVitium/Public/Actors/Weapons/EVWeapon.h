// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/EVEquipmentInitInterface.h"
#include "EVWeapon.generated.h"

class UEVWeaponTraceComponent;
class UEVWeaponDefinition;

UCLASS()
class EXVITIUM_API AEVWeapon : public AActor, public IEVEquipmentInitInterface
{
	GENERATED_BODY()

public:
	AEVWeapon();

	// IEVEquipmentInitInterface
	virtual void InitializeFromDefinition_Implementation(const UEVEquipmentDefinition* EquipmentDefinition) override;
	virtual void AttachShadowMesh_Implementation(USkeletalMeshComponent* ShadowMesh, FName ShadowMeshSocketName) override;
	
	const UEVWeaponDefinition* GetWeaponDefinition() const { return WeaponDefinition; };
	UStaticMeshComponent* GetWeaponMeshComponent() const { return WeaponMeshComponent; };
	USkeletalMeshComponent* GetWeaponSkeletalMeshComponent() const { return WeaponSkeletalMeshComponent; };

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EV|Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EV|Weapon")
	TObjectPtr<USkeletalMeshComponent> WeaponSkeletalMeshComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "EV|Weapon")
	TObjectPtr<UStaticMeshComponent> ShadowWeaponMeshComponent; // Shadow for weapon mesh

	UPROPERTY(VisibleAnywhere, Category = "EV|Weapon")
	TObjectPtr<USkeletalMeshComponent> ShadowWeaponSkeletalMeshComponent; // Shadow for weapon skeletal mesh
	
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "EV|Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<const UEVWeaponDefinition> WeaponDefinition;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EV|Collision", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEVWeaponTraceComponent> WeaponTraceComponent;
};
	