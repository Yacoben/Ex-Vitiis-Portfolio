// 


#include "Animation/EVAnimNotifyState_WeaponTrace.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameplayTags/EVGameplayTags.h"

void UEVAnimNotifyState_WeaponTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	SendGameplayEvent(MeshComp, EVTags::Events::Combat::WeaponCollisionStart);
}

void UEVAnimNotifyState_WeaponTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	SendGameplayEvent(MeshComp, EVTags::Events::Combat::WeaponCollisionEnd);
}

void UEVAnimNotifyState_WeaponTrace::SendGameplayEvent(USkeletalMeshComponent* MeshComp, const FGameplayTag& EventTag) const
{
	if (!MeshComp) return;
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);
	if (!ASC) return;

	FGameplayEventData EventData;
	EventData.EventTag = EventTag;
	ASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}
