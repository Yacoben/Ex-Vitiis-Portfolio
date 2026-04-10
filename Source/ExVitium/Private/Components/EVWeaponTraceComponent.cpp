#include "Components/EVWeaponTraceComponent.h"

#include "Components/MeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"


UEVWeaponTraceComponent::UEVWeaponTraceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEVWeaponTraceComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEVWeaponTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Trace();
}

void UEVWeaponTraceComponent::Initialize(UMeshComponent* InMesh, AActor* InOwnerCharacter)
{
	Mesh = InMesh;
	ActorsToAlwaysIgnore.AddUnique(GetOwner()); // Weapon actor
	if (InOwnerCharacter)
	{
		ActorsToAlwaysIgnore.AddUnique(InOwnerCharacter); // Character holding the weapon
	}
}

void UEVWeaponTraceComponent::StartTrace()
{
	bIsTracing = true;
	bHasPreviousPositions = false;
	PreviousFramePositions.Empty();
}

void UEVWeaponTraceComponent::EndTrace()
{
	bIsTracing = false;
	bHasPreviousPositions = false;
	PreviousFramePositions.Empty();
	AlreadyHitActors.Empty();
}

void UEVWeaponTraceComponent::Trace()
{	
	if (!bIsTracing) return;
	if (!Mesh) return;
	
	const FVector TraceStartSocketLoc = Mesh->GetSocketLocation(TraceStartSocket);
	const FVector TraceEndSocketLoc = Mesh->GetSocketLocation(TraceEndSocket);

	// Sample N points along the weapon (from base to tip)
	TArray<FVector> CurrentPositions;
	CurrentPositions.SetNum(TraceSegments);
	for (int32 i = 0; i < TraceSegments; i++)
	{
		const float Alpha = static_cast<float>(i) / static_cast<float>(TraceSegments - 1);
		CurrentPositions[i] = FMath::Lerp(TraceStartSocketLoc, TraceEndSocketLoc, Alpha);
	}

	// First frame of trace — just store positions, no sweep yet
	if (!bHasPreviousPositions)
	{
		PreviousFramePositions = CurrentPositions;
		bHasPreviousPositions = true;
		return;
	}

	// Build query params once
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(ActorsToAlwaysIgnore);
	for (const TWeakObjectPtr<AActor>& HitActor : AlreadyHitActors)
	{
		if (HitActor.IsValid())
		{
			QueryParams.AddIgnoredActor(HitActor.Get());
		}
	}

	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(SphereRadius);

	// Sweep each sample point from previous position → current position (covers the arc)
	for (int32 i = 0; i < TraceSegments; i++)
	{
		const FVector& PrevPos = PreviousFramePositions[i];
		const FVector& CurrPos = CurrentPositions[i];

		// Skip if the point barely moved (avoid zero-length sweeps)
		if (FVector::DistSquared(PrevPos, CurrPos) < 1.f) continue;

		TArray<FHitResult> HitResults;
		const bool bHit = GetWorld()->SweepMultiByChannel(
			HitResults,
			PrevPos,
			CurrPos,
			FQuat::Identity,
			ECollisionChannel::ECC_GameTraceChannel4,
			CollisionShape,
			QueryParams
		);

		if (bHit)
		{
			for (const FHitResult& HitResult : HitResults)
			{
				AActor* HitActor = HitResult.GetActor();
				if (HitActor && !AlreadyHitActors.Contains(HitActor))
				{
					AlreadyHitActors.Add(HitActor);
					QueryParams.AddIgnoredActor(HitActor); // Don't hit same actor from another segment
					OnWeaponTraceHit.Broadcast(HitResult);

#if ENABLE_DRAW_DEBUG
					if (bDebug)
					{
						DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, SphereRadius, 8, FColor::Yellow, false, 2.f, 0, 2.f);
					}
#endif
				}
			}
		}

#if ENABLE_DRAW_DEBUG
		if (bDebug)
		{
			const FColor TraceColor = bHit ? FColor::Red : FColor::Green;
			DrawDebugLine(GetWorld(), PrevPos, CurrPos, TraceColor, false, -1.f, 0, 1.f);
			DrawDebugSphere(GetWorld(), PrevPos, SphereRadius, 6, FColor::Blue, false, -1.f, 0, 0.5f);
			DrawDebugSphere(GetWorld(), CurrPos, SphereRadius, 6, TraceColor, false, -1.f, 0, 0.5f);
		}
#endif
	}

#if ENABLE_DRAW_DEBUG
	if (bDebug)
	{
		// Draw current weapon line for reference
		DrawDebugLine(GetWorld(), TraceStartSocketLoc, TraceEndSocketLoc, FColor::White, false, -1.f, 0, 0.5f);
	}
#endif

	// Store current positions for next frame
	PreviousFramePositions = CurrentPositions;
}
