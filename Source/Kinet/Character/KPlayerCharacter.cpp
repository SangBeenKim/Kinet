#include "Character/KPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Input/KInputConfig.h"
#include "Kismet/KismetSystemLibrary.h" //LOG
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/OverlapResult.h"
#include "Animation/KAnimInstance.h"
#include "Components/KStatusComponent.h"
#include "Items/KWeapon.h"
//#include "Interfaces/Interactable.h"

AKPlayerCharacter::AKPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.DoNotCreateDefaultSubobject(TEXT("HPBarWidgetComp")))
	, InteractRange(150.f)
	, InteractRadius(100.f)
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 400.f;
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bInheritYaw = true;
	SpringArmComp->bInheritPitch = true;
	SpringArmComp->bInheritRoll = false;
	SpringArmComp->bDoCollisionTest = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;
	
}

void AKPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AKPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (IsValid(EIC))
	{
		EIC->BindAction(InputConfig->Move, ETriggerEvent::Triggered, this, &ThisClass::InputMove);
		EIC->BindAction(InputConfig->Look, ETriggerEvent::Triggered, this, &ThisClass::InputLook);
		EIC->BindAction(InputConfig->Jump, ETriggerEvent::Started, this, &ThisClass::Jump);
		EIC->BindAction(InputConfig->Jump, ETriggerEvent::Completed, this, &ThisClass::StopJumping);
		EIC->BindAction(InputConfig->Attack_Melee, ETriggerEvent::Started, this, &ThisClass::InputAttack);
		EIC->BindAction(InputConfig->Interact, ETriggerEvent::Started, this, &ThisClass::InputInteract);
		EIC->BindAction(InputConfig->Aiming, ETriggerEvent::Started, this, &ThisClass::InputAiming);
		EIC->BindAction(InputConfig->Aiming, ETriggerEvent::Completed, this, &ThisClass::StopAiming);
		//Test
		EIC->BindAction(InputConfig->Test, ETriggerEvent::Started, this, &ThisClass::InputTest);
		//Test
	}
}

bool AKPlayerCharacter::CanJumpInternal_Implementation() const
{
	bool bCanJump = Super::CanJumpInternal_Implementation();

	if (StatusComp->bIsAiming == true)
	{
		bCanJump = false;
	}

	return bCanJump;

}

void AKPlayerCharacter::InputMove(const FInputActionValue& InValue)
{
	FVector2D MoveVector = InValue.Get<FVector2D>();

	const FRotator ControlRotation = GetController()->GetControlRotation();
	const FRotator ControlRotationYaw(0.f, ControlRotation.Yaw, 0.f);

	const FVector ForwardVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::X);
	const FVector RightVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardVector, MoveVector.X);
	AddMovementInput(RightVector, MoveVector.Y);
}

void AKPlayerCharacter::InputLook(const FInputActionValue& InValue)
{
	FVector2D LookVector = InValue.Get<FVector2D>();

	const float Sensitivity = 1.0f;

	AddControllerYawInput(LookVector.X * Sensitivity);
	AddControllerPitchInput(LookVector.Y * Sensitivity);
}

void AKPlayerCharacter::InputAttack()
{
	if (GetCharacterMovement()->IsFalling()) return;

	if (!IsValid(CurrentWeapon)) return; // 기본 공격 추가시 함수 종료대신 그걸로 대체
	
	if (StatusComp->bIsAiming == true)
	{
		CurrentWeapon->ExecuteAttackRanged();
		
		APlayerController* OwnerPlayerController = Cast<APlayerController>(GetController());
		if (IsValid(AttackRangedCameraShake) && IsValid(OwnerPlayerController))
		{
			OwnerPlayerController->ClientStartCameraShake(AttackRangedCameraShake);
		}
	}
	else
	{
		CurrentWeapon->ExecuteAttack();
	}
}

void AKPlayerCharacter::InputInteract()
{
	FVector ActorLocation = GetActorLocation();
	FVector Forward = GetActorForwardVector();
	FVector SphereCenter = ActorLocation + (Forward * InteractRange);

	TArray<FOverlapResult> Overlaps;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(InteractRadius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->OverlapMultiByChannel(
		Overlaps,
		SphereCenter,
		FQuat::Identity,
		ECC_Visibility,
		Sphere,
		Params
	);
	// 디버그 드로잉
	DrawDebugSphere(GetWorld(), SphereCenter, InteractRadius, 12, FColor::Green, false, 2.0f);

	if (bHit)
	{
		for (const FOverlapResult& Result : Overlaps)
		{
			AActor* HitActor = Result.GetActor();
			if (IInteractable* TargetActor = Cast<IInteractable>(HitActor))
			{
				TargetActor->Interact(this);
				break;
			}
		}
	}
}

void AKPlayerCharacter::InputAiming()
{
	if (GetCharacterMovement()->IsFalling()) return;

	if (!IsValid(CurrentWeapon)) return;

	SetCameraAimView(true);
	OnCombatModeChanged.Broadcast(true);
}

void AKPlayerCharacter::StopAiming()
{
	SetCameraAimView(false);
	OnCombatModeChanged.Broadcast(false);
}

void AKPlayerCharacter::SetCameraAimView(bool bIsAiming)
{
	if (bIsAiming)
	{
		StatusComp->bIsAiming = true;
		bUseControllerRotationYaw = true;
		SpringArmComp->TargetArmLength = 125.f;
		SpringArmComp->SetRelativeLocation(FVector(0.f, 50.f, 70.f));
		GetCharacterMovement()->MaxWalkSpeed = AimSpeed;
	}
	else
	{
		StatusComp->bIsAiming = false;
		bUseControllerRotationPitch = false;
		SpringArmComp->TargetArmLength = 400.f;
		SpringArmComp->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}

	UKismetSystemLibrary::PrintString(
		this,
		FString::Printf(TEXT("bIsAiming : %s"), StatusComp->bIsAiming ? TEXT("True") : TEXT("False"))
	); // LOG
}

void AKPlayerCharacter::InputTest()
{
	// TestLogic
}

void AKPlayerCharacter::Die()
{
	Super::Die();

	if (IsValid(GetController()))
	{
		GetController()->UnPossess();
	}
}


