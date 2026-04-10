// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "GameplayTags/EVGameplayTags.h"
#include "EVItemDefinition.generated.h"

class UTexture2D;

/**
 * Item definition is the base class for all items in the game.
 */
UCLASS()
class EXVITIUM_API UEVItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// Description
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Descritpion")
	FText DisplayName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Descritpion", meta = (MultiLine = true))
	FText Description;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Descritpion")
	TSoftObjectPtr<UTexture2D> Icon;
	
	//Stats
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Stats", meta = (ClampMin = "1"))
	int32 MaxStackSize = 1;
	
	// Category
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV|Tags|Category")
	FGameplayTag ItemCategory;
};
