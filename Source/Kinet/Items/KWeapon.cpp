#include "Items/KWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Character/KPlayerCharacter.h"
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

	if (AKPlayerCharacter* PlayerCharacter = Cast<AKPlayerCharacter>(Interactor))
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		AttachToComponent(PlayerCharacter->GetMesh(), AttachmentRules, FName(TEXT("hand_rSocket")));
		SetOwner(PlayerCharacter);
		SetActorEnableCollision(false);
		StaticMeshComp->SetSimulatePhysics(false);
		PlayerCharacter->SetCurrentWeapon(this);
		PlayerCharacter->OnAttackNotify.AddUObject(this, &ThisClass::HandleAttackSignal);
	}

}

UAnimMontage* AKWeapon::Attack()
{
	if (IsValid(AM_Attack)) return AM_Attack;

	return nullptr;
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
	}
	else if (SignalName == TEXT("MeleeAttack"))
	{
		CreateHitTrace();
	}
}

void AKWeapon::CreateHitTrace()
{
	// 소켓 위치 가져오기
	FVector Start = StaticMeshComp->GetSocketLocation(TEXT("S_Muzzle"));
	FVector End = StaticMeshComp->GetSocketLocation(TEXT("S_Grip_Bottom"));

	const float SphereRadius = 30.f;

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	// 휘두르는 궤적을 캡슐 형태로 체크
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(SphereRadius), // 반지름 20cm
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
					HitActor,       // 대미지 받을 액터
					10.f,           // 대미지 양
					nullptr,		// 공격자 컨트롤러
					this,           // 공격 유발 액터
					nullptr         // 대미지 타입 클래스
				);

				HitHistory.Add(HitActor);
			}
		}
	}

	// 실시간 확인용 디버그
	//DrawDebugSphere(GetWorld(), End, SphereRadius, 12, FColor::Cyan, false, 0.1f);
	// Sweep과 동일한 형태를 그리는 디버그 코드
	DrawDebugCapsule(
		GetWorld(),
		(Start + End) * 0.5f,           // 캡슐의 중심점
		(End - Start).Size() * 0.5f + SphereRadius, // 캡슐의 절반 높이 (두 지점 거리 + 반지름)
		SphereRadius,                    // 반지름
		FRotationMatrix::MakeFromZ(End - Start).ToQuat(), // 캡슐의 방향
		FColor::Green, false, 0.1f
	);

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

