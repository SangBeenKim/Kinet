#include "Character/KNonPlayerCharacter.h"
#include "Controller/KAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/KStatusComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/KPlayerCharacter.h"
#include "Animation/KAnimInstance.h"

AKNonPlayerCharacter::AKNonPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsNowAttacking(false)
{
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = AKAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

}

void AKNonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (false == IsPlayerControlled())
	{
		bUseControllerRotationYaw = false;

		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);
		GetCharacterMovement()->MaxWalkSpeed = 300.f;

	}

}

void AKNonPlayerCharacter::BeginAttack()
{
	UKAnimInstance* AnimInstance = Cast<UKAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance) == true, TEXT("Invalid AnimInstance"));

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	if (IsValid(AnimInstance) && IsValid(AttackMeleeMontage) && AnimInstance->Montage_IsPlaying(AttackMeleeMontage) == false)
	{
		AnimInstance->Montage_Play(AttackMeleeMontage);

		bIsNowAttacking = true;

		TestAttack();

		if (OnAttackMontageEndedDelegate.IsBound() == false)
		{
			OnAttackMontageEndedDelegate.BindUObject(this, &AKNonPlayerCharacter::EndAttack);
			AnimInstance->Montage_SetEndDelegate(OnAttackMontageEndedDelegate, AttackMeleeMontage);
		}

	}
}

void AKNonPlayerCharacter::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	bIsNowAttacking = false;

	if (OnAttackMontageEndedDelegate.IsBound() == true)
	{
		OnAttackMontageEndedDelegate.Unbind();
	}
}

void AKNonPlayerCharacter::Die()
{
	Super::Die();

	AKAIController* AIController = Cast<AKAIController>(GetController());
	if (IsValid(AIController))
	{
		AIController->EndAI();
	}
}
