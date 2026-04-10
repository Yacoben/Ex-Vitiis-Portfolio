// 


#include "Characters/EVPlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "Animation/EVAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/EVEquipmentManagerComponent.h"
#include "Components/EVFootstepComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTags/EVGameplayTags.h"
#include "PDA/EVItemDefinition.h"
#include "Player/EVPlayerState.h"


AEVPlayerCharacter::AEVPlayerCharacter()
{
	SetupConfig();
	
	EquipmentManager = CreateDefaultSubobject<UEVEquipmentManagerComponent>(TEXT("EquipmentManager"));
}

void AEVPlayerCharacter::SetupConfig()
{
	PrimaryActorTick.bCanEverTick = false;

	// Controller Rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw   = true;
	bUseControllerRotationRoll  = false;
	
	// Capsule Component
	GetCapsuleComponent()->InitCapsuleSize(34.0f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(FName("PlayerCapsule"));
	
	// FirstPersonCameraComponent
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->FieldOfView = 100.0f;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// Arms Mesh
	ArmsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmsMesh"));
	ArmsMesh->SetupAttachment(FirstPersonCameraComponent);
	ArmsMesh->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, -160.0f),
		FRotator(0.0f, -90.0f, 0.0f)
	);
	ArmsMesh->SetOnlyOwnerSee(true);
	ArmsMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	ArmsMesh->SetCollisionProfileName(FName("NoCollision"));
	ArmsMesh->CastShadow = false;
	ArmsMesh->bCastDynamicShadow = false;

	// Full Body Mesh
	GetMesh()->SetupAttachment(GetCapsuleComponent());
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, -96.0f),
		FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;
	GetMesh()->SetCollisionProfileName(FName("CharacterMesh"));
	GetMesh()->bCastHiddenShadow = true;
	GetMesh()->bHiddenInGame = true;
	
	/* Character Movement Component */
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxStepHeight = 45.0f;  
	GetCharacterMovement()->SetWalkableFloorAngle(45.0f);   

	// Walking
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;  
	GetCharacterMovement()->MaxAcceleration = 2048.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.0f;
	GetCharacterMovement()->GroundFriction = 8.0f;
	GetCharacterMovement()->bUseSeparateBrakingFriction = false;

	// Crouching
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(58.0f);
	GetCharacterMovement()->MaxWalkSpeedCrouched = 300.0f;   

	// Jumping/Falling
	GetCharacterMovement()->JumpZVelocity = 600.0f;   
	GetCharacterMovement()->GravityScale = 1.75f;    
	GetCharacterMovement()->AirControl = 0.1f;    
	GetCharacterMovement()->AirControlBoostMultiplier = 2.0f;
	GetCharacterMovement()->AirControlBoostVelocityThreshold = 25.0f;
	GetCharacterMovement()->BrakingDecelerationFalling = 0.0f; 
	GetCharacterMovement()->FallingLateralFriction = 0.0f;
	/* Character GetCharacterMovement() Component */
}

void AEVPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitializeAbilitySystem();
	GrantDefaultAbilities();
	ApplyDefaultEffects();
	InitializeAnimInstance();
}

void AEVPlayerCharacter::InitializeAbilitySystem()
{
	if (!IsValid(GetAbilitySystemComponent())) return;
	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
}

void AEVPlayerCharacter::InitializeAnimInstance() const
{
	if (UEVAnimInstance* AnimInstance = Cast<UEVAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
		{
			AnimInstance->InitializeWithAbilitySystem(ASC);
		}
	}
}

UAbilitySystemComponent* AEVPlayerCharacter::GetAbilitySystemComponent() const
{
	const AEVPlayerState* EVPlayerState = Cast<AEVPlayerState>(GetPlayerState());
	if (!IsValid(EVPlayerState)) return nullptr;
	
	return EVPlayerState->GetAbilitySystemComponent();
}


bool AEVPlayerCharacter::TryEquip_Implementation(UEVItemDefinition* ItemDefinition, const FGameplayTag EquipmentSlot)
{
	if (!IsValid(ItemDefinition)) return false;
	if (!IsValid(GetAbilitySystemComponent())) return false;
	
	FGameplayEventData EventData;
	EventData.OptionalObject = ItemDefinition; // Item info PDA
	EventData.EventTag = EVTags::Events::Equipment::Equip; // Event tag
	EventData.TargetTags.AddTag(EquipmentSlot); // Tag for equipment slot
	if (GetAbilitySystemComponent()->HandleGameplayEvent(EventData.EventTag, &EventData) > 0) // If gameplay ability start at least 1 return true
	{
		return true;
	}
	
	return false;
}

bool AEVPlayerCharacter::TryUnequip_Implementation(const FGameplayTag EquipmentSlot)
{
	if (!IsValid(GetAbilitySystemComponent())) return false;
	
	FGameplayEventData EventData;
	EventData.EventTag = EVTags::Events::Equipment::Unequip; // Event tag
	EventData.TargetTags.AddTag(EquipmentSlot); // Tag for equipment slot
	if (GetAbilitySystemComponent()->HandleGameplayEvent(EventData.EventTag, &EventData) > 0) // If gameplay ability start at least 1 return true
	{
		return true;
	}
	
	return false;
}


