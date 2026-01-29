#include "Character/KCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/HPBar.h"
#include "Animation/KAnimInstance.h"
#include "Kismet/GameplayStatics.h"

AKCharacterBase::AKCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	float CharacterHalfHeight = 90.f;
	float CharacterRadius = 40.f;

	GetCapsuleComponent()->InitCapsuleSize(CharacterRadius, CharacterHalfHeight);

	FVector PivotPosition(0.f, 0.f, -CharacterHalfHeight);
	FRotator PivotRotation(0.f, -90.f, 0.f);
	GetMesh()->SetRelativeLocationAndRotation(PivotPosition, PivotRotation);

	GetCharacterMovement()->MaxWalkSpeed = 350.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	HPBarWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidgetComp"));
	HPBarWidgetComp->SetupAttachment(GetMesh());
	HPBarWidgetComp->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	HPBarWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	HPBarWidgetComp->SetDrawSize(FVector2D(150.f, 20.f));

	bIsDead = false;
}

float AKCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (bIsDead) return 0.f;

	CurrentHP = FMath::Clamp(CurrentHP - FinalDamageAmount, 0.f, MaxHP);

	SetCurrentHP(CurrentHP);

	if (CurrentHP <= 0.f)
	{
		Die();
		return FinalDamageAmount;
	}

	UKAnimInstance* AnimInstance = Cast<UKAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance) && IsValid(TakeDamageMontage) && !AnimInstance->Montage_IsPlaying(TakeDamageMontage))
	{
		AnimInstance->Montage_Play(TakeDamageMontage);
	}

	return FinalDamageAmount;
}

void AKCharacterBase::SetMaxHP(float InMaxHP)
{
	MaxHP = InMaxHP;

	if (MaxHP < KINDA_SMALL_NUMBER)
	{
		MaxHP = 0.1f;
	}

	OnMaxHPChanged.Broadcast(MaxHP);
}

void AKCharacterBase::SetCurrentHP(float InCurrentHP)
{
	CurrentHP = InCurrentHP;
	if (CurrentHP <= KINDA_SMALL_NUMBER)
	{
		CurrentHP = 0.f;
		OnOutOfCurrentHP.Broadcast();
	}
	OnCurrentHPChanged.Broadcast(CurrentHP);
}

void AKCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	UUserWidget* WidgetInstance = HPBarWidgetComp->GetUserWidgetObject();
	if (IsValid(WidgetInstance))
	{
		UHPBar* HPBarWidget = Cast<UHPBar>(WidgetInstance);
		if (IsValid(HPBarWidget))
		{
			HPBarWidget->InitializeHPBarWidget(this);
		}
	}
	

}

void AKCharacterBase::Die()
{
	if (bIsDead) return;

	bIsDead = true;

	UKAnimInstance* AnimInstance = Cast<UKAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance) && IsValid(DeathMontage) && !AnimInstance->Montage_IsPlaying(DeathMontage))
	{
		AnimInstance->Montage_Play(DeathMontage);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (IsValid(GetController()))
	{
		GetController()->UnPossess();
	}

	SetLifeSpan(1.f);
}

void AKCharacterBase::InputAttackMelee()
{
	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}
	
	UKAnimInstance* AnimInstance = Cast<UKAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance) && IsValid(AttackMeleeMontage) && !AnimInstance->Montage_IsPlaying(AttackMeleeMontage))
	{
		AnimInstance->Montage_Play(AttackMeleeMontage);
		TestAttack();
	}
}

void AKCharacterBase::TestAttack()
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

