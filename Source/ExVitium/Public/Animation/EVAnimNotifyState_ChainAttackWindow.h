// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Animation/AnimNotifyQueue.h"
#include "EVAnimNotifyState_ChainAttackWindow.generated.h"

/**
 * 
 */
UCLASS()
class EXVITIUM_API UEVAnimNotifyState_ChainAttackWindow : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
	virtual FString GetNotifyName_Implementation() const override {return TEXT("Chain Attack Window");};

private:
	void SendGameplayEvent(USkeletalMeshComponent* MeshComp, const FGameplayTag& EventTag) const;
};
