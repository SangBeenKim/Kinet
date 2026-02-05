#include "Items/KWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "Character/KCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"

int32 AKWeapon::ShowAttackRangedDebug = 0;

FAutoConsoleVariableRef CVarShowAttackRangedDebug(
	TEXT("Kinet.ShowAttackRangedDebug"),
	AKWeapon::ShowAttackRangedDebug,
	TEXT(""),
	ECVF_Cheat
);

AKWeapon::AKWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	SetRootComponent(StaticMeshComp);

}

void AKWeapon::Interact(AActor* Interactor)
{
	if (Owner != nullptr) return;

	if (AKCharacterBase* OwnerCharacter = Cast<AKCharacterBase>(Interactor))
	{
		SetOwner(OwnerCharacter);
		SetInstigator(OwnerCharacter);
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

void AKWeapon::EquipWeapon(AKCharacterBase* InCharacter)
{
	if (!IsValid(InCharacter)) return;

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(InCharacter->GetMesh(), AttachmentRules, FName(TEXT("hand_rSocket")));
	SetActorEnableCollision(false);
	StaticMeshComp->SetSimulatePhysics(false);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	InCharacter->SetCurrentWeapon(this);
	InCharacter->OnAttackNotify.RemoveAll(this);
	InCharacter->OnAttackNotify.AddUObject(this, &ThisClass::HandleAttackSignal);

	if (IsValid(AM_GetWeapon))
	{
		InCharacter->PlayAnimMontage(AM_GetWeapon);
	}

}

void AKWeapon::UnequipWeapon()
{
	if (AKCharacterBase* OwnerCharacter = Cast<AKCharacterBase>(GetOwner()))
	{
		OwnerCharacter->OnAttackNotify.RemoveAll(this);
	}

	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	DetachFromActor(DetachRules);

	SetActorEnableCollision(true);
	if (StaticMeshComp)
	{
		StaticMeshComp->SetSimulatePhysics(true);
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	SetOwner(nullptr);
	SetInstigator(nullptr);
}

void AKWeapon::DestroyWeapon(AKCharacterBase* InCharacter)
{
	if (GetOwner() != InCharacter) return;

	UnequipWeapon();
	
	Destroy();

}

void AKWeapon::ExecuteAttackRanged()
{
	if (AKCharacterBase* OwnerCharacter = Cast<AKCharacterBase>(GetOwner()))
	{
		if (IsValid(AM_AttackRanged))
		{
			OwnerCharacter->PlayAnimMontage(AM_AttackRanged);
			TryFire();

		}
	}

}

void AKWeapon::BeginPlay()
{
	Super::BeginPlay();

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

void AKWeapon::TryFire()
{
	AKCharacterBase* PlayerCharacter = Cast<AKCharacterBase>(GetOwner());
	APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
	if (IsValid(PlayerController) == true)
	{
		float FocalDistance = 400.f;
		FVector FocalLocation;
		FVector CameraLocation;
		FRotator CameraRotation;

		PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector AimDirectionFromCamera = CameraRotation.Vector().GetSafeNormal();
		FocalLocation = CameraLocation + (AimDirectionFromCamera * FocalDistance);

		FVector WeaponMuzzleLocation = StaticMeshComp->GetSocketLocation(TEXT("S_Muzzle"));
		FVector FinalFocalLocation = FocalLocation + (((WeaponMuzzleLocation - FocalLocation) | AimDirectionFromCamera) * AimDirectionFromCamera);

		FTransform TargetTransform = FTransform(CameraRotation, FinalFocalLocation);

		if (1 == ShowAttackRangedDebug)
		{
			DrawDebugSphere(GetWorld(), WeaponMuzzleLocation, 2.f, 16, FColor::Red, false, 60.f);

			DrawDebugSphere(GetWorld(), CameraLocation, 2.f, 16, FColor::Yellow, false, 60.f);

			DrawDebugSphere(GetWorld(), FinalFocalLocation, 2.f, 16, FColor::Magenta, false, 60.f);

			// (WeaponLoc - FocalLoc)
			DrawDebugLine(GetWorld(), FocalLocation, WeaponMuzzleLocation, FColor::Yellow, false, 60.f, 0, 2.f);

			// AimDir
			DrawDebugLine(GetWorld(), CameraLocation, FinalFocalLocation, FColor::Blue, false, 60.f, 0, 2.f);

			// Project Direction Line
			DrawDebugLine(GetWorld(), WeaponMuzzleLocation, FinalFocalLocation, FColor::Red, false, 60.f, 0, 2.f);
		}

		FVector BulletDirection = TargetTransform.GetUnitAxis(EAxis::X);
		FVector StartLocation = WeaponMuzzleLocation;
		FVector EndLocation = TargetTransform.GetLocation() + BulletDirection * MaxAttackRange;

		FHitResult HitResult;
		FCollisionQueryParams TraceParams(NAME_None, false, this);
		TraceParams.AddIgnoredActor(this);
		TraceParams.AddIgnoredActor(PlayerCharacter);

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult, 
			StartLocation, 
			EndLocation, 
			ECC_Pawn, // 콜리전 채널
			TraceParams
		);

		if (bHit == false)
		{
			HitResult.TraceStart = StartLocation;
			HitResult.TraceEnd = EndLocation;
		}

		if (2 == ShowAttackRangedDebug)
		{
			if (bHit == true)
			{
				DrawDebugSphere(GetWorld(), StartLocation, 2.f, 16, FColor::Red, false, 60.f);
				DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 2.f, 16, FColor::Green, false, 60.f);
				DrawDebugLine(GetWorld(), StartLocation, HitResult.ImpactPoint, FColor::Blue, false, 60.f, 0, 2.f);
			}
			else
			{
				DrawDebugSphere(GetWorld(), StartLocation, 2.f, 16, FColor::Red, false, 60.f);
				DrawDebugSphere(GetWorld(), EndLocation, 2.f, 16, FColor::Green, false, 60.f);
				DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, false, 60.f, 0, 2.f);
			}
		}

		if (bHit == true)
		{
			UGameplayStatics::ApplyDamage(
				HitResult.GetActor(),						// 대미지 받을 액터
				10.f,										// 대미지 양
				GetOwner()->GetInstigatorController(),		// 공격자 컨트롤러
				GetOwner(),									// 공격 유발 액터
				nullptr										// 대미지 타입 클래스
			);
			/*AKCharacterBase* HittedCharacter = Cast<AKCharacterBase>(HitResult.GetActor());
			if (IsValid(HittedCharacter))
			{
				FDamageEvent DamageEvent;
				HittedCharacter->TakeDamage(10.f, DamageEvent, PlayerController, this);
			}*/
		}
	}

}

