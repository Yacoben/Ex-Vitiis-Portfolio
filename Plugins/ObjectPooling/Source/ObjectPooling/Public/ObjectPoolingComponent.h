// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectPoolingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPoolInitialized, const TArray<AActor*>&, ObjectPool);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPoolDeinitialized);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAcquireObject, AActor*, ActorFromPool, const FTransform&, SpawnTransform);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReturnObject);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class OBJECTPOOLING_API UObjectPoolingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UObjectPoolingComponent();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	
	/* Delegates ------------------------------------------------------------ */
	
	// Broadcast when the pool has been initialized with all actors
	UPROPERTY(BlueprintAssignable, Category = "Object Pooling")
	FOnPoolInitialized OnPoolInitialized;
	
	// Broadcast when the pool has been deinitialized and all actors destroyed
	UPROPERTY(BlueprintAssignable, Category = "Object Pooling")
	FOnPoolDeinitialized OnPoolDeinitialized;
	
	// Broadcast when an actor is acquired from the pool with its spawn transform
	UPROPERTY(BlueprintAssignable, Category = "Object Pooling")
	FOnAcquireObject OnAcquireObject;
	
	// Broadcast when an actor is returned back to the pool
	UPROPERTY(BlueprintAssignable, Category = "Object Pooling")
	FOnReturnObject OnReturnObject;
	
	/* Delegates ------------------------------------------------------------ */
	
	
	/* Configuration ------------------------------------------------------------ */
	
	// Whether to automatically initialize pool on BeginPlays
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Pooling")
	bool bInitializeOnBeginPlay = true;
	
	// Actor class to spawn in the pool
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Pooling")
	TSubclassOf<AActor> ObjectClass = nullptr;
	
	// Number of actors to pre-spawn in the pool
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Pooling", meta = (ClampMin = 1, ClampMax = 100))
	int32 PoolSize = 1;
	
	// Location where inactive actors will be waiting for acquisition
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Pooling")
	FVector ParkingZoneLocation = FVector(0, 0, -10000);

	// Spawns actors and fills the pool
	UFUNCTION(BlueprintCallable, Category = "Object Pooling")
	void InitializePool();
	
	// Destroys all pooled actors and clears the pool
	UFUNCTION(BlueprintCallable, Category = "Object Pooling")
	void DeinitializePool();

	// Acquires an inactive actor from the pool, activates it at the given transform, and returns it. Returns nullptr if no actors are available.
	UFUNCTION(BlueprintCallable, Category = "Object Pooling")
    AActor* AcquireObject(const FTransform& SpawnTransform);
	
	// Deactivates the actor and returns it back to the inactive pool
	UFUNCTION(BlueprintCallable, Category = "Object Pooling")
	void ReturnObject(AActor* Actor);
    
	/* Configuration ------------------------------------------------------------ */
    
private:
	UPROPERTY()
	TArray<AActor*> InactiveObjectPool;
	
	UPROPERTY()
	TArray<AActor*> ActiveObjectPool;
	
	bool bInitialized = false;
	FTransform ParkingZoneTransform;
	
	// Enables visibility, tick, and collision, then moves the actor to the given transform
	void ActivateActor(AActor* Actor, const FTransform& Transform);
	
	// Disables visibility, tick, and collision, then moves the actor to the parking zone
	void DeactivateActor(AActor* Actor) const;
	
	/* Getters and Setters */
public:
	// Get array of inactive objects in pool
	UFUNCTION(BlueprintCallable, Category = "Object Pooling")
	const TArray<AActor*>& GetInactiveObjectPool() const { return InactiveObjectPool; }
	
	// Get array of active objects in pool
	UFUNCTION(BlueprintCallable, Category = "Object Pooling")
	const TArray<AActor*>& GetActiveObjectPool() const { return ActiveObjectPool; }
};

