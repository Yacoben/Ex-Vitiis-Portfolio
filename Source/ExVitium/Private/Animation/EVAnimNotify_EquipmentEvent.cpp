// 


#include "Animation/EVAnimNotify_EquipmentEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameplayTags/EVGameplayTags.h"


void UEVAnimNotify_EquipmentEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);
	if (!ASC) return;

	FGameplayEventData EventData;
	EventData.EventTag = EVTags::Events::Equipment::MontageNotify;
	ASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}


