// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ObjectPoolingInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UPoolableActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * Optional interface for pooled actors.
 * Implement in C++ or Blueprint to get callbacks on acquire/return.
 */
class OBJECTPOOLING_API IPoolableActor
{
	GENERATED_BODY()

public:
	/** Called when the actor is taken from the pool. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Object Pooling")
	void OnAcquiredFromPool(const FTransform& SpawnTransform, class UObjectPoolingComponent* OwningPool);

	/** Called when the actor is returned to the pool. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Object Pooling")
	void OnReturnedToPool();
};

