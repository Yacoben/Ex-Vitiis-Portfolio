// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/HitResult.h"
#include "GameplayTagContainer.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "EVFootstepComponent.generated.h"

class UEVEquipmentManagerComponent;
class AEVPlayerCharacter;
class AEVPlayerController;
class UCharacterMovementComponent;
class ACharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class EXVITIUM_API UEVFootstepComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEVFootstepComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Locomotion|Event")
	void OnStartMoving(float Speed, EPhysicalSurface SurfaceType, float ChestWeight, float FeetWeight);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Locomotion|Event")
	void OnMoving(float Speed, EPhysicalSurface SurfaceType, float ChestWeight, float FeetWeight);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Locomotion|Event")
	void OnStopMoving(float Speed, EPhysicalSurface SurfaceType, float ChestWeight, float FeetWeight);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Locomotion|Event")
	void OnLanded(float FallSpeed, EPhysicalSurface SurfaceType, float ChestWeight, float FeetWeight);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Locomotion|Event")
	void OnJumped(float Speed, EPhysicalSurface SurfaceType, float ChestWeight, float FeetWeight);
	
	UFUNCTION(BlueprintPure, Category = "Cached")
	AEVPlayerCharacter* GetOwnerCharacter() {return OwnerCharacter;}
	
	UFUNCTION(BlueprintPure, Category = "Cached")
	UCharacterMovementComponent* GetMovementComponent() {return OwnerMovementComponent;}
	
	UFUNCTION(BlueprintPure, Category = "Cached")
	AEVPlayerController* GetOwnerController() {return OwnerController;}

	// Line traces down from character's feet. Updates CurrentSurfaceType / SurfaceHitResult. Called automatically before footstep events.
	UFUNCTION(BlueprintCallable, Category = "Locomotion|Surface")
	EPhysicalSurface TraceSurface();
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	float CurrentSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	float PreviousSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	FVector CurrentLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	FVector PreviousLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	bool bIsMoving = false;
	
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	bool bIsStartedMoving = false;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	bool bWasMovingBeforeAir = false;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	float DistanceAccumulator = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|Surface")
	FHitResult SurfaceHitResult;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion|Tweakable", meta = (Tooltip = "Distance between footstep events"))
	float StepDistance = 130.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion|Tweakable", meta = (Tooltip = "Speed required to trigger OnStartMoving"))
	float StartMovingThreshold = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion|Tweakable", meta = (Tooltip = "Below this speed the character is considered stopped"))
	float MovingThreshold = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion|Tweakable", meta = (Tooltip = "How far below the feet to trace for surface detection"))
	float TraceDistance = 30.f;
	
private:
	// Internal handler to LandedDelegate from MovementComponent
	UFUNCTION()
	void HandleLanded(const FHitResult& Hit);
	
	// Internal handler to JumpedDelegate from OwnerController
	UFUNCTION()
	void HandleJumped();

	// Returns the weight of the item in the given slot, or 0 if the slot is empty
	float GetEquippedSlotWeight(const FGameplayTag& Slot) const;

	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> CurrentSurfaceType = SurfaceType_Default;

	UPROPERTY()
	TObjectPtr<AEVPlayerCharacter> OwnerCharacter = nullptr;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwnerMovementComponent = nullptr;
	
	UPROPERTY()
	TObjectPtr<AEVPlayerController> OwnerController = nullptr;
	
	UPROPERTY()
	TObjectPtr<UEVEquipmentManagerComponent> OwnerEquipment = nullptr;
	
};
