// Copyright Epic Games, Inc. All Rights Reserved.

#include "ObjectPoolingComponent.h"
#include "ObjectPoolingInterface.h"
#include "ObjectPooling.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"


UObjectPoolingComponent::UObjectPoolingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UObjectPoolingComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (bInitializeOnBeginPlay)
	{
		InitializePool();
	}
}

void UObjectPoolingComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DeinitializePool();
	
	Super::EndPlay(EndPlayReason);
}

void UObjectPoolingComponent::InitializePool()
{
	if (bInitialized) return;
	if (!IsValid(GetWorld())) return;
	ensureMsgf(IsValid(ObjectClass), TEXT("ObjectClass is nullptr. Check the Object Pooling Component settings."));
	if (!IsValid(ObjectClass)) return;
	
	ParkingZoneTransform = FTransform(FQuat::Identity, ParkingZoneLocation);
	
	for (int32 i = 0 ; i < PoolSize; i++)
	{
		if (AActor* ActorDeferred = GetWorld()->SpawnActorDeferred<AActor>(ObjectClass, ParkingZoneTransform, GetOwner()))
		{
			ActorDeferred->FinishSpawning(ParkingZoneTransform);
			DeactivateActor(ActorDeferred);
			InactiveObjectPool.AddUnique(ActorDeferred);
		}
	}
	
	bInitialized = true;
	OnPoolInitialized.Broadcast(InactiveObjectPool);
}

void UObjectPoolingComponent::DeinitializePool()
{
	if (!bInitialized) return;
	if (!IsValid(GetWorld())) return;
	
	for (int32 i = 0 ; i < InactiveObjectPool.Num(); i++)
	{
		if (InactiveObjectPool[i])
		{
			InactiveObjectPool[i]->Destroy();
		}
	}
	
	for (int32 i = 0 ; i < ActiveObjectPool.Num(); i++)
	{
		if (ActiveObjectPool[i])
		{
			ActiveObjectPool[i]->Destroy();
		}
	}
	
	InactiveObjectPool.Empty();
	ActiveObjectPool.Empty();
	bInitialized = false;
	OnPoolDeinitialized.Broadcast();
}

AActor* UObjectPoolingComponent::AcquireObject(const FTransform& SpawnTransform)
{
	if (!bInitialized) return nullptr;
	if (InactiveObjectPool.IsEmpty()) return nullptr;
	if (!IsValid(InactiveObjectPool.Last())) return nullptr;
	
	AActor* Actor = InactiveObjectPool.Last();
	ActivateActor(Actor, SpawnTransform);
	ActiveObjectPool.AddUnique(Actor);
	InactiveObjectPool.Pop();
	OnAcquireObject.Broadcast(Actor, SpawnTransform);
	if (Actor->Implements<UPoolableActor>())
	{
		IPoolableActor::Execute_OnAcquiredFromPool(Actor, SpawnTransform, this);
	}
	return Actor;
}

void UObjectPoolingComponent::ReturnObject(AActor* Actor)
{
	if (!bInitialized) return;
	if (!IsValid(Actor)) return;
	if (!ActiveObjectPool.Contains(Actor)) return;
	
	ActiveObjectPool.Remove(Actor);
	InactiveObjectPool.AddUnique(Actor);
	DeactivateActor(Actor);
	OnReturnObject.Broadcast();
	if (Actor->Implements<UPoolableActor>())
	{
		IPoolableActor::Execute_OnReturnedToPool(Actor);
	}
}

void UObjectPoolingComponent::ActivateActor(AActor* Actor, const FTransform& Transform)
{
	if (!IsValid(Actor)) return;
	
	Actor->SetActorTransform(Transform);
	Actor->SetActorHiddenInGame(false);
	Actor->SetActorTickEnabled(true);
	Actor->SetActorEnableCollision(true);
}

void UObjectPoolingComponent::DeactivateActor(AActor* Actor) const
{
	if (!IsValid(Actor)) return;
	
	Actor->SetActorHiddenInGame(true);
	Actor->SetActorTickEnabled(false);
	Actor->SetActorEnableCollision(false);
	Actor->SetActorTransform(ParkingZoneTransform);
}

