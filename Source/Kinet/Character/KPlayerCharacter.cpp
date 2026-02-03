#include "Character/KPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Input/KInputConfig.h"
#include "Kismet/KismetSystemLibrary.h" //LOG
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/OverlapResult.h"
#include "Items/KWeapon.h"
#include "Interfaces/Interactable.h"
#include "Animation/KAnimInstance.h"

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

void AKPlayerCharacter::SetCurrentWeapon(AKWeapon* InWeapon)
{
	if (!IsValid(InWeapon) || CurrentWeapon == InWeapon) return;

	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->SetOwner(nullptr);
	}

	CurrentWeapon = InWeapon;
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
		EIC->BindAction(InputConfig->Attack_Melee, ETriggerEvent::Started, this, &ThisClass::InputAttackMelee);
		EIC->BindAction(InputConfig->Interact, ETriggerEvent::Started, this, &ThisClass::InputInteract);
		//Test
		EIC->BindAction(InputConfig->Test, ETriggerEvent::Started, this, &ThisClass::InputTest);
		//Test
	}
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
				UKAnimInstance* AnimInstance = Cast<UKAnimInstance>(GetMesh()->GetAnimInstance());
				if (IsValid(AnimInstance) && IsValid(GetPistol) && !AnimInstance->Montage_IsPlaying(GetPistol))
				{
					AnimInstance->Montage_Play(GetPistol);
				}
				break;
			}
		}
	}
}

void AKPlayerCharacter::InputTest()
{
	if (!IsValid(CurrentWeapon)) return;

	UAnimMontage* AttackMontage = CurrentWeapon->Attack();
	if (IsValid(AttackMontage))
	{
		UKAnimInstance* AnimInstance = Cast<UKAnimInstance>(GetMesh()->GetAnimInstance());
		if (IsValid(AnimInstance) && !AnimInstance->Montage_IsPlaying(AttackMontage))
		{
			AnimInstance->Montage_Play(AttackMontage);
		}
	}
}

void AKPlayerCharacter::Die()
{
	Super::Die();

	if (IsValid(GetController()))
	{
		GetController()->UnPossess();
	}
}


