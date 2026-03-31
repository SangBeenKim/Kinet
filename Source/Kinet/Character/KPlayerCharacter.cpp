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
#include "Interfaces/Interactable.h"
#include "Components/ParkourComponent.h"
#include "Components/CapsuleComponent.h"

bool AKPlayerCharacter::bShowPlayerCharacterDebug = false;

FAutoConsoleVariableRef CVarShowPlayerCharacterDebug(
	TEXT("Kinet.ShowPlayerCharacterDebug"),
	AKPlayerCharacter::bShowPlayerCharacterDebug,
	TEXT(""),
	ECVF_Cheat
);

AKPlayerCharacter::AKPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.DoNotCreateDefaultSubobject(TEXT("HPBarWidgetComp")))
	, InteractRange(150.f)
	, InteractRadius(100.f)
	, DefaultLength(400.f)
	, AimLength(125.f)
	, AimCameraPos(FVector(0.f,50.f,70.f))
	, DefaultFOV(90.f)
	, AimFOV(70.f)
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
	SpringArmComp->TargetArmLength = DefaultLength;
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bInheritYaw = true;
	SpringArmComp->bInheritPitch = true;
	SpringArmComp->bInheritRoll = false;
	SpringArmComp->bDoCollisionTest = true;
	SpringArmComp->bEnableCameraLag = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;
	CameraComp->FieldOfView = DefaultFOV;

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
		EIC->BindAction(InputConfig->Dash, ETriggerEvent::Started, this, &ThisClass::InputDash);
		EIC->BindAction(InputConfig->Test, ETriggerEvent::Started, this, &ThisClass::InputParkour);
	}
}

void AKPlayerCharacter::OnMontageEnded(UAnimMontage* InMontage, bool bInterrupted)
{
	StatusComp->bIsDash = false;

	Super::OnMontageEnded(InMontage, bInterrupted);
}

void AKPlayerCharacter::InputMove(const FInputActionValue& InValue)
{
	if (!IsValid(GetController()) || StatusComp->bIsDash == true) return;

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

	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}

void AKPlayerCharacter::InputAttack()
{
	if (GetCharacterMovement()->IsFalling()) return;

	if (!IsValid(CurrentWeapon)) return;
	
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

	if (bShowPlayerCharacterDebug)
	{
		// 디버그 드로잉
		DrawDebugSphere(GetWorld(), SphereCenter, InteractRadius, 12, FColor::Green, false, 2.0f);
	}

}

void AKPlayerCharacter::InputAiming()
{
	if (!IsValid(CurrentWeapon) || GetCharacterMovement()->IsFalling() || StatusComp->bIsActionLocked == true) return;

	SetCameraAimView(true);
	OnCombatModeChanged.Broadcast(true);
}

void AKPlayerCharacter::StopAiming()
{
	SetCameraAimView(false);
	OnCombatModeChanged.Broadcast(false);
}

void AKPlayerCharacter::InputDash()
{
	if (StatusComp->bIsDash == true || StatusComp->bIsActionLocked == true || GetCharacterMovement()->IsFalling()) return;

	StatusComp->bIsDash = true;

	const FVector FacingDirection = GetActorForwardVector();
	const FVector MoveDirection = GetMovementComponent()->GetLastInputVector();

	EDashDirection DashAnimMontageDirection;
	FVector DashVelocity;

	if (MoveDirection.IsNearlyZero())
	{
		DashAnimMontageDirection = EDashDirection::Forward;
		DashVelocity = FacingDirection * DashForce;
	}
	else
	{
		DashAnimMontageDirection = SelectDirectionalMontage(FacingDirection, MoveDirection);
		DashVelocity = MoveDirection.GetSafeNormal() * DashForce;
	}

	UAnimMontage* DashAnimMontage = AM_Dash.FindRef(DashAnimMontageDirection);
	
	TSharedPtr<FRootMotionSource_ConstantForce> ConstantForce = MakeShared<FRootMotionSource_ConstantForce>();
	ConstantForce->InstanceName = TEXT("DashForce");
	ConstantForce->AccumulateMode = ERootMotionAccumulateMode::Override;
	ConstantForce->Priority = 5;
	ConstantForce->Duration = 0.25f;
	ConstantForce->Force = DashVelocity;

	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		PlayAnimMontage(DashAnimMontage);
		MovementComp->ApplyRootMotionSource(ConstantForce);
	}
	
}

void AKPlayerCharacter::SetCameraAimView(bool bIsAiming)
{
	if (bIsAiming)
	{
		StatusComp->bIsAiming = true;
		bUseControllerRotationYaw = true;
		SpringArmComp->TargetArmLength = AimLength;
		SpringArmComp->SetRelativeLocation(AimCameraPos);
		CameraComp->FieldOfView = AimFOV;
	}
	else
	{
		StatusComp->bIsAiming = false;
		bUseControllerRotationYaw = false;
		SpringArmComp->TargetArmLength = DefaultLength;
		SpringArmComp->SetRelativeLocation(FVector::ZeroVector);
		CameraComp->FieldOfView = DefaultFOV;
	}

	if (bShowPlayerCharacterDebug)
	{
		UKismetSystemLibrary::PrintString(
			this,
			FString::Printf(TEXT("bIsAiming : %s"), StatusComp->bIsAiming ? TEXT("True") : TEXT("False"))
		); // LOG
	}
}

void AKPlayerCharacter::InputParkour()
{
	if (ParkourComp->DoingParkourAction()) return;

	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	if (!IsValid(MovementComp) || !IsValid(CapsuleComp)) return;

	FParkourCheckResult ParkourCheckResult;
	FParkourCheckInputs ParkourCheckInputs;
	ParkourCheckInputs.ActorLocation = GetActorLocation();
	ParkourCheckInputs.TraceForwardDirection = GetActorForwardVector();
	ParkourCheckInputs.CapsuleHalfHeight = CapsuleComp->GetScaledCapsuleHalfHeight();
	ParkourCheckInputs.CapsuleRadius = CapsuleComp->GetScaledCapsuleRadius();

	switch (MovementComp->MovementMode)
	{
	case EMovementMode::MOVE_Falling:
	case EMovementMode::MOVE_Flying:
		// Falling, Flying 에선 기본값 사용
		break;
	default:
		const FVector LocalVelocity = GetActorRotation().UnrotateVector(MovementComp->Velocity);
		const float InputForwardDistance = FMath::GetMappedRangeValueClamped(
			FVector2D(0.f, 500.f),
			FVector2D(75.f, 350.f),
			LocalVelocity.X
		);
		ParkourCheckInputs.TraceForwardDistance = InputForwardDistance;
	}

	bool bParkourSucceeded = ParkourComp->TryParkourAction(ParkourCheckInputs, ParkourCheckResult);

}

void AKPlayerCharacter::Die()
{
	Super::Die();

	//if (IsValid(GetController()))
	//{
	//	GetController()->UnPossess();
	//}
}


