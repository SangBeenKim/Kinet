#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "KAnimInstance.generated.h"

class AKCharacterBase;
class UCharacterMovementComponent;

UCLASS()
class KINET_API UKAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AKCharacterBase> OwnerCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovement;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector Velocity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float GroundSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 bShouldMove : 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 bIsFalling : 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 bIsDead : 1;

};
