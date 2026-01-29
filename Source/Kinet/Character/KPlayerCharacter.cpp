#include "Character/KPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Input/KInputConfig.h"
#include "Kismet/KismetSystemLibrary.h" //LOG
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/KAnimInstance.h"
#include "Kismet/GameplayStatics.h"

AKPlayerCharacter::AKPlayerCharacter()
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

float AKPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (GetCharacterMovement()->IsFalling())
	{
		return FinalDamageAmount;
	}

	UKAnimInstance* AnimInstance = Cast<UKAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance) && IsValid(TakeDamageMontage) && !AnimInstance->Montage_IsPlaying(TakeDamageMontage))
	{
		AnimInstance->Montage_Play(TakeDamageMontage);
		UKismetSystemLibrary::PrintString(
			this,
			TEXT("TakeDamage")
		);
	}

	return FinalDamageAmount;
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

void AKPlayerCharacter::InputAttackMelee(const FInputActionValue& InValue)
{
	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}

	TestAttack();

	UKAnimInstance* AnimInstance = Cast<UKAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance) && IsValid(AttackMeleeMontage) && !AnimInstance->Montage_IsPlaying(AttackMeleeMontage))
	{
		AnimInstance->Montage_Play(AttackMeleeMontage);
	}

}

void AKPlayerCharacter::TestAttack()
{
	FVector ForwardVector = GetActorForwardVector();
	FVector StartLocation = GetActorLocation() + (ForwardVector * 100.f); // 캐릭터 앞 100cm 지점
	float AttackRadius = 50.f; // 구체 반지름

	// 2. 충돌 검사 결과 및 무시할 대상 설정
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// 3. 스피어 트레이스 실행 (Pawn 채널 기준)
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		StartLocation,
		StartLocation, // 시작과 끝을 같게 하면 그 지점에 구체를 생성함
		FQuat::Identity,
		ECC_Pawn, // 기본 Pawn 채널 사용
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	// 디버그용 구체 그리기 (테스트용)
	FColor DebugColor = bHit ? FColor::Green : FColor::Red;
	DrawDebugSphere(GetWorld(), StartLocation, AttackRadius, 12, DebugColor, false, 1.f);

	// 4. 대미지 전달
	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor)
			{
				UGameplayStatics::ApplyDamage(
					HitActor,       // 대미지 받을 액터
					10.f,           // 대미지 양
					GetController(),// 공격자 컨트롤러
					this,           // 공격 유발 액터
					nullptr         // 대미지 타입 클래스
				);
			}
		}
	}
}

