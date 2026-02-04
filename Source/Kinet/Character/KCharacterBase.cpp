#include "Character/KCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/HPBar.h"
#include "Animation/KAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/KStatusComponent.h"
#include "Items/KWeapon.h"

AKCharacterBase::AKCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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
	
	StatusComp = CreateDefaultSubobject<UKStatusComponent>(TEXT("StatusComp"));

	HPBarWidgetComp = ObjectInitializer.CreateOptionalDefaultSubobject<UWidgetComponent>(this, TEXT("HPBarWidgetComp"));
	if (IsValid(HPBarWidgetComp))
	{
		HPBarWidgetComp->SetupAttachment(GetMesh());
		HPBarWidgetComp->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
		HPBarWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
		HPBarWidgetComp->SetDrawSize(FVector2D(150.f, 20.f));
	}
}

void AKCharacterBase::SetCurrentWeapon(AKWeapon* InWeapon)
{
	if (!IsValid(InWeapon) || CurrentWeapon == InWeapon) return;

	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->SetOwner(nullptr);
	}

	CurrentWeapon = InWeapon;
}

void AKCharacterBase::PlayAnimMontage(UAnimMontage* InMontage)
{
	if (IsValid(InMontage))
	{
		if (IsValid(CharacterAnim) && !CharacterAnim->Montage_IsPlaying(InMontage))
		{
			StatusComp->bIsActionLocked = true;
			CharacterAnim->Montage_Play(InMontage);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AKCharacterBase::OnMontageEnded);
			CharacterAnim->Montage_SetEndDelegate(EndDelegate, InMontage);
		}
	}
}

void AKCharacterBase::OnMontageEnded(UAnimMontage* InMontage, bool bInterrupted)
{
	StatusComp->bIsActionLocked = false;

}

float AKCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	StatusComp->ApplyDamage(FinalDamageAmount);

	if (StatusComp->IsDead() == true)
	{
		Die();
		return FinalDamageAmount;
	}

	if (IsValid(CharacterAnim) && IsValid(TakeDamageMontage) && !CharacterAnim->Montage_IsPlaying(TakeDamageMontage))
	{
		CharacterAnim->Montage_Play(TakeDamageMontage);
	}

	return FinalDamageAmount;
}

void AKCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(HPBarWidgetComp))
	{
		UUserWidget* WidgetInstance = HPBarWidgetComp->GetUserWidgetObject();
		if (IsValid(WidgetInstance))
		{
			UHPBar* HPBarWidget = Cast<UHPBar>(WidgetInstance);
			if (IsValid(HPBarWidget))
			{
				HPBarWidget->InitializeHPBarWidget(StatusComp);
			}
		}
	}

	CharacterAnim = Cast<UKAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(CharacterAnim), TEXT("[%s] AnimInstance is invalid."), *GetName());

}

void AKCharacterBase::Die()
{
	if (IsValid(CharacterAnim) && IsValid(DeathMontage) && !CharacterAnim->Montage_IsPlaying(DeathMontage))
	{
		CharacterAnim->Montage_Play(DeathMontage);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(1.f);

}

void AKCharacterBase::InputAttackMelee()
{
	if (GetCharacterMovement()->IsFalling()) return;

	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->ExecuteAttack();
	}

}
