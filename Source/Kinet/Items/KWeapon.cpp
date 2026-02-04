#include "Items/KWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Character/KCharacterBase.h"
#include "Kismet/GameplayStatics.h"

AKWeapon::AKWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	SetRootComponent(StaticMeshComp);

	DetectPlayerCharacterComp = CreateDefaultSubobject<USphereComponent>(TEXT("DetectPlayerCharacterComp"));
	DetectPlayerCharacterComp->SetupAttachment(StaticMeshComp);

}

void AKWeapon::Interact(AActor* Interactor)
{
	if (Owner != nullptr) return;

	if (AKCharacterBase* OwnerCharacter = Cast<AKCharacterBase>(Interactor))
	{
		SetOwner(OwnerCharacter);
		EquipWeapon(OwnerCharacter);
	}

}

void AKWeapon::ExecuteAttack()
{
	if (AKCharacterBase* OwnerCharacter = Cast<AKCharacterBase>(GetOwner()))
	{
		if (IsValid(AM_Attack))
		{
			OwnerCharacter->PlayAnimMontage(AM_Attack);
		}
	}
}

UAnimMontage* AKWeapon::Attack()
{
	if (IsValid(AM_Attack)) return AM_Attack;

	return nullptr;
}

void AKWeapon::EquipWeapon(AKCharacterBase* InCharacter)
{
	if (!IsValid(InCharacter)) return;

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(InCharacter->GetMesh(), AttachmentRules, FName(TEXT("hand_rSocket")));
	SetActorEnableCollision(false);
	StaticMeshComp->SetSimulatePhysics(false);
	InCharacter->SetCurrentWeapon(this);
	if (!InCharacter->OnAttackNotify.IsBound())
	{
		InCharacter->OnAttackNotify.AddUObject(this, &ThisClass::HandleAttackSignal);
	}
}

void AKWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (!ensureMsgf(IsValid(DetectPlayerCharacterComp), TEXT("SphereComponent is invalid.")))
	{
		return;
	}

	if (!DetectPlayerCharacterComp->OnComponentBeginOverlap.IsAlreadyBound(this, &ThisClass::OnOverlapBegin))
	{
		DetectPlayerCharacterComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	}

	if (!DetectPlayerCharacterComp->OnComponentEndOverlap.IsAlreadyBound(this, &ThisClass::OnOverlapEnd))
	{
		DetectPlayerCharacterComp->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnd);
	}

}

void AKWeapon::HandleAttackSignal(FName SignalName)
{
	if (SignalName == TEXT("AttackBegin"))
	{
		ResetHitHistory();
		LastMuzzlePos = StaticMeshComp->GetSocketLocation(TEXT("S_Muzzle"));
		LastGripPos = StaticMeshComp->GetSocketLocation(TEXT("S_Grip_Bottom"));
	}
	else if (SignalName == TEXT("MeleeAttack"))
	{
		CreateHitTrace();
	}
}

void AKWeapon::CreateHitTrace()
{
	FVector CurrentMuzzlePos = StaticMeshComp->GetSocketLocation(TEXT("S_Muzzle"));
	FVector CurrentGripPos = StaticMeshComp->GetSocketLocation(TEXT("S_Grip_Bottom"));

	FVector LastCenter = (LastMuzzlePos + LastGripPos) * 0.5f;
	FVector CurrentCenter = (CurrentMuzzlePos + CurrentGripPos) * 0.5f;

	const float SphereRadius = 30.f;
	const float HalfHeight = (CurrentMuzzlePos - CurrentGripPos).Size() * 0.5f;

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		LastCenter,
		CurrentCenter,
		FRotationMatrix::MakeFromZ(CurrentMuzzlePos - CurrentGripPos).ToQuat(),
		ECC_Pawn, // 콜리전 채널
		FCollisionShape::MakeCapsule(SphereRadius, HalfHeight),
		Params
	);

	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor && !HitHistory.Contains(HitActor))
			{
				UGameplayStatics::ApplyDamage(
					HitActor,									// 대미지 받을 액터
					10.f,										// 대미지 양
					GetOwner()->GetInstigatorController(),		// 공격자 컨트롤러
					GetOwner(),									// 공격 유발 액터
					nullptr										// 대미지 타입 클래스
				);

				HitHistory.Add(HitActor);
			}
		}
	}

	LastMuzzlePos = CurrentMuzzlePos;
	LastGripPos = CurrentGripPos;

	// 실시간 확인용 디버그
	DrawDebugCapsule(
		GetWorld(),
		(LastCenter + CurrentCenter) * 0.5f,								// 캡슐의 중심점
		(CurrentCenter - LastCenter).Size() * 0.5f + HalfHeight,			// 캡슐의 절반 높이 (두 지점 거리 + 반지름)
		SphereRadius,														// 반지름
		FRotationMatrix::MakeFromZ(CurrentCenter - LastCenter).ToQuat(),	// 캡슐의 방향
		FColor::Green, false, 0.1f
	); /**/

}

void AKWeapon::ResetHitHistory()
{
	HitHistory.Empty();
}

void AKWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// UI 위젯 표시
}

void AKWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// UI 위젯 숨김
}

