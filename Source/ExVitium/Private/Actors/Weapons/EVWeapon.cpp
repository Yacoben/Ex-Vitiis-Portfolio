

#include "Actors/Weapons/EVWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/EVWeaponTraceComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "PDA/EVEquipmentDefinition.h"
#include "PDA/EVWeaponDefinition.h"


AEVWeapon::AEVWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	
	WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMeshComponent);
	WeaponMeshComponent->SetCollisionProfileName(FName("NoCollision"));
	WeaponMeshComponent->SetGenerateOverlapEvents(false);
	WeaponMeshComponent->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	WeaponMeshComponent->SetVisibility(false);
	
	WeaponSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponSkeletalMesh"));
	WeaponSkeletalMeshComponent->SetupAttachment(GetRootComponent());
	WeaponSkeletalMeshComponent->SetCollisionProfileName(FName("NoCollision"));
	WeaponSkeletalMeshComponent->SetGenerateOverlapEvents(false);
	WeaponSkeletalMeshComponent->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	WeaponSkeletalMeshComponent->SetVisibility(false);
	
	// Shadow only static mesh
	ShadowWeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShadowStaticMesh"));
	ShadowWeaponMeshComponent->SetupAttachment(GetRootComponent());
	ShadowWeaponMeshComponent->SetCollisionProfileName(FName("NoCollision"));
	ShadowWeaponMeshComponent->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;
	ShadowWeaponMeshComponent->bHiddenInGame = true;
	ShadowWeaponMeshComponent->bCastHiddenShadow = false;
	ShadowWeaponMeshComponent->SetVisibility(false);

	// Shadow only skeletal mesh
	ShadowWeaponSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShadowSkeletalMesh"));
	ShadowWeaponSkeletalMeshComponent->SetupAttachment(GetRootComponent());
	ShadowWeaponSkeletalMeshComponent->SetCollisionProfileName(FName("NoCollision"));
	ShadowWeaponSkeletalMeshComponent->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;
	ShadowWeaponSkeletalMeshComponent->bHiddenInGame = true;
	ShadowWeaponSkeletalMeshComponent->bCastHiddenShadow = false;
	ShadowWeaponSkeletalMeshComponent->SetVisibility(false);
	
	// Trace Component for collision
	WeaponTraceComponent = CreateDefaultSubobject<UEVWeaponTraceComponent>(TEXT("WeaponTraceComponent"));
}

void AEVWeapon::InitializeFromDefinition_Implementation(const UEVEquipmentDefinition* EquipmentDefinition)
{
	if (!IsValid(EquipmentDefinition)) return;

	// Casts to WeaponDefinition for weapon specific data
	WeaponDefinition = Cast<UEVWeaponDefinition>(EquipmentDefinition);
	if (!WeaponDefinition) return;
	
	if (!EquipmentDefinition->EquipmentSkeletalMesh.IsNull())
	{
		if (USkeletalMesh* LoadedMesh = EquipmentDefinition->EquipmentSkeletalMesh.LoadSynchronous())
		{
			WeaponSkeletalMeshComponent->SetSkeletalMesh(LoadedMesh);
			WeaponSkeletalMeshComponent->SetVisibility(true);
			WeaponMeshComponent->SetVisibility(false);
			WeaponTraceComponent->Initialize(WeaponSkeletalMeshComponent, GetOwner());
			return;
		}
	}

	if (!EquipmentDefinition->EquipmentMesh.IsNull())
	{
		if (UStaticMesh* LoadedMesh = EquipmentDefinition->EquipmentMesh.LoadSynchronous())
		{
			WeaponMeshComponent->SetStaticMesh(LoadedMesh);
			WeaponMeshComponent->SetVisibility(true);
			WeaponSkeletalMeshComponent->SetVisibility(false);
			WeaponTraceComponent->Initialize(WeaponMeshComponent, GetOwner());
		}
	}
}

void AEVWeapon::AttachShadowMesh_Implementation(USkeletalMeshComponent* ShadowMesh, FName ShadowMeshSocketName)
{
	if (!ShadowMesh) return;

	if (WeaponSkeletalMeshComponent->IsVisible() && ShadowWeaponSkeletalMeshComponent)
	{
		ShadowWeaponSkeletalMeshComponent->SetSkeletalMesh(
			WeaponSkeletalMeshComponent->GetSkeletalMeshAsset());
		ShadowWeaponSkeletalMeshComponent->bCastHiddenShadow = true;
		ShadowWeaponSkeletalMeshComponent->AttachToComponent(
			ShadowMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, ShadowMeshSocketName);
	}
	else if (WeaponMeshComponent->IsVisible() && ShadowWeaponMeshComponent)
	{
		ShadowWeaponMeshComponent->SetStaticMesh(WeaponMeshComponent->GetStaticMesh());
		ShadowWeaponMeshComponent->bCastHiddenShadow = true;
		ShadowWeaponMeshComponent->AttachToComponent(
			ShadowMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, ShadowMeshSocketName);
	}
}


