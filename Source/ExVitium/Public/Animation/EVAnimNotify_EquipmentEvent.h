// 

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "EVAnimNotify_EquipmentEvent.generated.h"

/**
 * Anim Notify that sends a GameplayEvent (EVTags.Events.Equipment.MontageNotify) through the ASC.
 * Place this in equip/unequip montages at the frame where the weapon should appear/disappear.
 * The equip/unequip ability listens for this event and spawns/destroys the actor accordingly.
 */
UCLASS(DisplayName = "EV Equipment Event")
class EXVITIUM_API UEVAnimNotify_EquipmentEvent : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override { return TEXT("Equipment Event"); }
};

