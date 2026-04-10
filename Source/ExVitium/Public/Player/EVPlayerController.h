// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EVPlayerController.generated.h"

struct FGameplayTag;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpedDelegate);

UCLASS()
class EXVITIUM_API AEVPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AEVPlayerController();
	
	FJumpedDelegate JumpedDelegate;
	
protected:
	virtual void SetupInputComponent() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "EV|Input")
	TArray<TObjectPtr<UInputMappingContext>> InputMappingContexts;
	
	UPROPERTY(EditDefaultsOnly, Category = "EV|Input|Movement")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "EV|Input|Movement")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "EV|Input|Movement")
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "EV|Input|Combat")
    TObjectPtr<UInputAction> LightAttackAction;
	
	void Jump();
	void StopJumping();
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void LightAttack(const FInputActionValue& Value);
	
	void ActivateAbility(const FGameplayTag& AbilityTag) const;
};
