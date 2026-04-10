// 


#include "Components/EVFootstepComponent.h"

#include "Characters/EVPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/EVEquipmentManagerComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTags/EVGameplayTags.h"
#include "PDA/EVEquipmentDefinition.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Player/EVPlayerController.h"


UEVFootstepComponent::UEVFootstepComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEVFootstepComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Cache AEVPlayerCharacter
	OwnerCharacter = Cast<AEVPlayerCharacter>(GetOwner());
	
	if (OwnerCharacter)
	{
		OwnerCharacter->LandedDelegate.RemoveDynamic(this, &ThisClass::HandleLanded);
		OwnerCharacter->LandedDelegate.AddDynamic(this, &ThisClass::HandleLanded);
		
		// Cache AEVPlayerCharacter's MovementComponent
		OwnerMovementComponent = OwnerCharacter->GetCharacterMovement();
		
		// Cache AEVPlayerController
		OwnerController = Cast<AEVPlayerController>(OwnerCharacter->GetController());
		
		if (OwnerController)
		{
			OwnerController->JumpedDelegate.RemoveDynamic(this, &ThisClass::HandleJumped);
			OwnerController->JumpedDelegate.AddDynamic(this, &ThisClass::HandleJumped);
		}
		
		// Cache Player Equipment
		OwnerEquipment = OwnerCharacter->GetEquipmentManager();
		
		// Set default values
		CurrentLocation = OwnerCharacter->GetActorLocation();
		PreviousLocation = CurrentLocation;
	}
}

void UEVFootstepComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (OwnerCharacter)
	{
		OwnerCharacter->LandedDelegate.RemoveDynamic(this, &ThisClass::HandleLanded);
	}
	
	if (OwnerController)
	{
		OwnerController->JumpedDelegate.RemoveDynamic(this, &ThisClass::HandleJumped);
	}
	
	Super::EndPlay(EndPlayReason);
}


float UEVFootstepComponent::GetEquippedSlotWeight(const FGameplayTag& Slot) const
{
	if (!OwnerEquipment) return 0.f;
	
	const FEVEquippedItemEntry* Entry = OwnerEquipment->GetEquippedItem(Slot);
	if (!Entry || !Entry->Definition) return 0.f;
	
	return Entry->Definition->Weight;
}

void UEVFootstepComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!OwnerCharacter) return;
	if (!OwnerMovementComponent) return;

	PreviousLocation = CurrentLocation;
	PreviousSpeed = CurrentSpeed;
	
	CurrentLocation = OwnerCharacter->GetActorLocation();
	CurrentSpeed = OwnerMovementComponent->Velocity.Size2D();
	bIsFalling = OwnerMovementComponent->IsFalling();
	bIsMoving = !OwnerMovementComponent->GetCurrentAcceleration().IsNearlyZero() && CurrentSpeed >= MovingThreshold;

	if (bIsFalling) return;
	
	// Start Moving — player is actively inputting movement and speed reached threshold
	if (bIsMoving && CurrentSpeed >= StartMovingThreshold && !bIsStartedMoving)
	{
		bIsStartedMoving = true;
		DistanceAccumulator = 0.0f;
		TraceSurface();
		OnStartMoving(CurrentSpeed, CurrentSurfaceType, GetEquippedSlotWeight(EVTags::EquipmentSlot::Chest), GetEquippedSlotWeight(EVTags::EquipmentSlot::Feet));
	}
	// Moving — accumulate distance, fire OnMoving every StepDistance
	else if (bIsStartedMoving && CurrentSpeed >= MovingThreshold)
	{
		const float Dist = FVector::Dist2D(CurrentLocation, PreviousLocation);
		DistanceAccumulator += Dist;
		
		if (DistanceAccumulator >= StepDistance)
		{
			DistanceAccumulator = 0.0f;
			TraceSurface();
			OnMoving(CurrentSpeed, CurrentSurfaceType, GetEquippedSlotWeight(EVTags::EquipmentSlot::Chest), GetEquippedSlotWeight(EVTags::EquipmentSlot::Feet));
		}
	}
	// Stop Moving — was moving but speed dropped below threshold
	else if (bIsStartedMoving && CurrentSpeed < MovingThreshold)
	{
		bIsStartedMoving = false;
		DistanceAccumulator = 0.0f;
		TraceSurface();
		OnStopMoving(CurrentSpeed, CurrentSurfaceType, GetEquippedSlotWeight(EVTags::EquipmentSlot::Chest), GetEquippedSlotWeight(EVTags::EquipmentSlot::Feet));
	}
}

void UEVFootstepComponent::OnStartMoving_Implementation(float Speed, EPhysicalSurface SurfaceType, float ChestWeight, float FeetWeight)
{
}

void UEVFootstepComponent::OnMoving_Implementation(float Speed, EPhysicalSurface SurfaceType, float ChestWeight, float FeetWeight)
{
}

void UEVFootstepComponent::OnStopMoving_Implementation(float Speed, EPhysicalSurface SurfaceType, float ChestWeight, float FeetWeight)
{
}

void UEVFootstepComponent::OnLanded_Implementation(float FallSpeed, EPhysicalSurface SurfaceType, float ChestWeight, float FeetWeight)
{
}

void UEVFootstepComponent::HandleLanded(const FHitResult& Hit)
{
	// Capture fall speed (absolute value of vertical velocity at impact)
	const float LandingFallSpeed = OwnerMovementComponent ? FMath::Abs(OwnerMovementComponent->Velocity.Z) : 0.0f;
	
	DistanceAccumulator = 0.0f;

	const bool bHasInput = OwnerMovementComponent && !OwnerMovementComponent->GetCurrentAcceleration().IsNearlyZero();

	if (bWasMovingBeforeAir && bHasInput)
	{
		// Keep bIsStartedMoving true, tick resumes Moving loop
	}
	else
	{
		// Reset to idle if wasn't moving before air or released keys mid air
		bIsStartedMoving = false;
	}

	TraceSurface();
	OnLanded(LandingFallSpeed, CurrentSurfaceType, GetEquippedSlotWeight(EVTags::EquipmentSlot::Chest), GetEquippedSlotWeight(EVTags::EquipmentSlot::Feet));
}

void UEVFootstepComponent::HandleJumped()
{
	bWasMovingBeforeAir = bIsStartedMoving;
	TraceSurface();
	OnJumped(CurrentSpeed, CurrentSurfaceType, GetEquippedSlotWeight(EVTags::EquipmentSlot::Chest), GetEquippedSlotWeight(EVTags::EquipmentSlot::Feet));
}

void UEVFootstepComponent::OnJumped_Implementation(float Speed, EPhysicalSurface SurfaceType, float ChestWeight, float FeetWeight)
{
	
}

EPhysicalSurface UEVFootstepComponent::TraceSurface()
{
	CurrentSurfaceType = SurfaceType_Default;
	SurfaceHitResult.Reset();

	if (!OwnerCharacter) return CurrentSurfaceType;

	const UCapsuleComponent* Capsule = OwnerCharacter->GetCapsuleComponent();
	if (!Capsule) return CurrentSurfaceType;

	const float CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	const float CapsuleRadius = Capsule->GetScaledCapsuleRadius();
	const FVector FeetLocation = OwnerCharacter->GetActorLocation() - FVector(0.0f, 0.0f, CapsuleHalfHeight);
	const FVector TraceEnd = FeetLocation - FVector(0.0f, 0.0f, TraceDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.AddIgnoredActor(OwnerCharacter);

	const FCollisionShape Sphere = FCollisionShape::MakeSphere(CapsuleRadius);
	
	// ECC_GameTraceChannel3 = Footstep trace channel - defined in Project Settings
	if (GetWorld()->SweepSingleByChannel(SurfaceHitResult, FeetLocation, TraceEnd, FQuat::Identity, ECC_GameTraceChannel3, Sphere, QueryParams))
	{
		if (const UPhysicalMaterial* PhysMat = SurfaceHitResult.PhysMaterial.Get())
		{
			CurrentSurfaceType = PhysMat->SurfaceType;
		}
	}

	return CurrentSurfaceType;
}

