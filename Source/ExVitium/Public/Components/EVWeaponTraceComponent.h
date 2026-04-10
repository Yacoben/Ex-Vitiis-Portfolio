// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EVWeaponTraceComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponTraceHit, const FHitResult&, HitResult);

class UMeshComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class EXVITIUM_API UEVWeaponTraceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEVWeaponTraceComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnWeaponTraceHit OnWeaponTraceHit;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug")
	bool bDebug = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trace")
	bool bIsTracing = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Socket")
	FName TraceStartSocket = FName("TraceStart");
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Socket")
	FName TraceEndSocket = FName("TraceEnd");
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision")
	float SphereRadius = 8.f;
	
	/** Number of sample points along the weapon blade. More = more accurate, more expensive. 3-4 is standard for souls-likes. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision", meta = (ClampMin = "2", ClampMax = "10"))
	int32 TraceSegments = 4;
	
	void Initialize(UMeshComponent* InMesh, AActor* InOwnerCharacter);
	void StartTrace();
	void EndTrace();
	void Trace();
	void SetDebug(bool bEnable) { bDebug = bEnable; }

private:
	UPROPERTY()
	TObjectPtr<UMeshComponent> Mesh;
	
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> AlreadyHitActors;
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> ActorsToAlwaysIgnore;

	/** Previous frame sample positions along the weapon. Used to sweep the arc between frames. */
	TArray<FVector> PreviousFramePositions;
	
	/** Whether we have valid previous frame data (false on first trace frame). */
	bool bHasPreviousPositions = false;
};
