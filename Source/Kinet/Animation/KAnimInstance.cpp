#include "Animation/KAnimInstance.h"
#include "Character/KNonPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UKAnimInstance::NativeInitializeAnimation()
{
	APawn* OwnerPawn = TryGetPawnOwner();
	if (IsValid(OwnerPawn))
	{
		OwnerCharacter = Cast<AKCharacterBase>(OwnerPawn);
		OwnerCharacterMovement = OwnerCharacter->GetCharacterMovement();
	}
}

void UKAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (IsValid(OwnerCharacter) == true && IsValid(OwnerCharacterMovement) == true)
	{
		Velocity = OwnerCharacterMovement->Velocity;
		GroundSpeed = Velocity.Size2D();

		float GroundAcceleration = OwnerCharacterMovement->GetCurrentAcceleration().Size2D();
		bool bIsAccelerationNearlyZero = FMath::IsNearlyZero(GroundAcceleration);
		bShouldMove = (KINDA_SMALL_NUMBER < GroundSpeed) && (bIsAccelerationNearlyZero == false);

		if (AKNonPlayerCharacter* OwnerNPC = Cast<AKNonPlayerCharacter>(OwnerCharacter))
		{
			bShouldMove = KINDA_SMALL_NUMBER < GroundSpeed;
		}

		bIsFalling = OwnerCharacterMovement->IsFalling();
	}
}
