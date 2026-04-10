// 


#include "Animation/EVAnimNotifyState_ChainAttackWindow.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameplayTags/EVGameplayTags.h"

void UEVAnimNotifyState_ChainAttackWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	SendGameplayEvent(MeshComp, EVTags::Events::Combat::ChainAttackWindowOpened);
}

void UEVAnimNotifyState_ChainAttackWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	SendGameplayEvent(MeshComp, EVTags::Events::Combat::ChainAttackWindowClosed);
}

void UEVAnimNotifyState_ChainAttackWindow::SendGameplayEvent(USkeletalMeshComponent* MeshComp, const FGameplayTag& EventTag) const
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
