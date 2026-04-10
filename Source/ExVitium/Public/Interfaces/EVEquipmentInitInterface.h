// 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EVEquipmentInitInterface.generated.h"

class UEVEquipmentDefinition;

UINTERFACE(MinimalAPI)
class UEVEquipmentInitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actors that can be initialized with equipment data (weapons, shields, etc.).
 * 
 */
class EXVITIUM_API IEVEquipmentInitInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Equipment")
	void InitializeFromDefinition(const UEVEquipmentDefinition* EquipmentDefinition);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Equipment")
	void AttachShadowMesh(USkeletalMeshComponent* ShadowMesh, FName ShadowMeshSocketName);
};

