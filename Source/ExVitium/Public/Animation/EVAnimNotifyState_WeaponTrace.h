// 

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "EVAnimNotifyState_WeaponTrace.generated.h"

struct FGameplayTag;

UCLASS()
class EXVITIUM_API UEVAnimNotifyState_WeaponTrace : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
	virtual FString GetNotifyName_Implementation() const override { return TEXT("Weapon Trace Window"); };

private:
	void SendGameplayEvent(USkeletalMeshComponent* MeshComp, const FGameplayTag& EventTag) const;
};
