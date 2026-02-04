#include "Character/KNonPlayerCharacter.h"
#include "Controller/KAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/KStatusComponent.h"
#include "Items/KWeapon.h"

AKNonPlayerCharacter::AKNonPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = AKAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

}

void AKNonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(DefaultWeaponClass))
	{
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = GetInstigator();

		AKWeapon* SpawnedWeapon = GetWorld()->SpawnActor<AKWeapon>(DefaultWeaponClass, Params);
		if (SpawnedWeapon)
		{
			SpawnedWeapon->SetActorHiddenInGame(true);
			SpawnedWeapon->EquipWeapon(this);
			SpawnedWeapon->SetActorHiddenInGame(false);
		}
	}

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
	if (!IsValid(CurrentWeapon)) return;

	StatusComp->bIsActionLocked = true;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	CurrentWeapon->ExecuteAttack();

}

void AKNonPlayerCharacter::OnMontageEnded(UAnimMontage* InMontage, bool bInterrupted)
{
	StatusComp->bIsActionLocked = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
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
