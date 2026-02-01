#include "Character/KNonPlayerCharacter.h"
#include "Controller/KAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/KStatusComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/KPlayerCharacter.h"

AKNonPlayerCharacter::AKNonPlayerCharacter()
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

float AKNonPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (StatusComp->IsDead() == true)
	{
		AKAIController* AIController = Cast<AKAIController>(GetController());
		if (IsValid(AIController))
		{
			AIController->EndAI();
		}
	}

	return FinalDamageAmount;
}
