#include "Components/ParkourComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MotionWarpingComponent.h"
#include "Gimmick/ParkourableActor.h"
#include "Kinet.h"

UParkourComponent::UParkourComponent()
	: bDoingParkourAction(false)
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UParkourComponent::SetCharacterProperties(const FCharacterPropertiesForParkour& InCharacterProperties)
{
	if (!IsValid(InCharacterProperties.Capsule) || !IsValid(InCharacterProperties.Anim) || !IsValid(InCharacterProperties.Movement) || !IsValid(InCharacterProperties.MotionWarping)) return;

	CharacterProperties = InCharacterProperties;
}

bool UParkourComponent::TryParkourAction(const FParkourCheckInputs& InParkourCheckInputs, FParkourCheckResult& OutParkourCheckResult)
{
	check(IsValid(CharacterProperties.MotionWarping));

	if (AActor* Owner = GetOwner())
	{
		FHitResult HitResult;
		const FVector TraceStart = InParkourCheckInputs.ActorLocation;
		const FVector TraceEnd = TraceStart + (InParkourCheckInputs.TraceForwardDirection * InParkourCheckInputs.TraceForwardDistance);
		const float CachedCapsuleRadius = InParkourCheckInputs.CapsuleRadius;
		const float CachedCapsuleHalfHeight = InParkourCheckInputs.CapsuleHalfHeight;

		// 파쿠르 액터 찾기
		if (!GenerateCapsuleTrace(HitResult, TraceStart, TraceEnd, CachedCapsuleRadius, CachedCapsuleHalfHeight, true))
		{
			return false;
		}

		CachedTarget = nullptr;
		if (AParkourableActor* ParkourableActor = Cast<AParkourableActor>(HitResult.GetActor()))
		{
			CachedTarget = ParkourableActor;
			if (!ParkourableActor->GetLedgeTransforms(HitResult.ImpactPoint, InParkourCheckInputs.ActorLocation, OutParkourCheckResult))
			{
				UE_LOG(LogTemp, Warning, TEXT("ParkourableActor Ledge is invalid."));
				CachedTarget = nullptr;
				return false;
			}
		}

		// 캐릭터와 액터 사이 장애물 감지
		const float Offset = 2.f;
		const FVector FrontHorizontalOffset = OutParkourCheckResult.FrontLedgeNormal * (InParkourCheckInputs.CapsuleRadius + Offset);
		const float VerticalOffsetZ = InParkourCheckInputs.CapsuleHalfHeight + Offset;

		FVector HasRoomCheck_FrontLedgeLocation = OutParkourCheckResult.FrontLedgeLocation + FrontHorizontalOffset;
		HasRoomCheck_FrontLedgeLocation.Z += VerticalOffsetZ;

		if (GenerateCapsuleTrace(HitResult, InParkourCheckInputs.ActorLocation, HasRoomCheck_FrontLedgeLocation, CachedCapsuleRadius, CachedCapsuleHalfHeight))
		{
			return false;
		}

		// 난간 높이 구하기
		const float GroundHeightFromActor = InParkourCheckInputs.ActorLocation.Z - InParkourCheckInputs.CapsuleHalfHeight;
		const float LedgeHeight = FMath::Abs(GroundHeightFromActor - OutParkourCheckResult.FrontLedgeLocation.Z);
		OutParkourCheckResult.ObstacleHeight = LedgeHeight;

		// 난간 위 장애물 감지
		const FVector BackHorizontalOffset = OutParkourCheckResult.BackLedgeNormal * (InParkourCheckInputs.CapsuleRadius + Offset);
		FVector HasRoomCheck_BackLedgeLocation = OutParkourCheckResult.BackLedgeLocation + BackHorizontalOffset;
		HasRoomCheck_BackLedgeLocation.Z += VerticalOffsetZ;

		if (GenerateCapsuleTrace(HitResult, HasRoomCheck_FrontLedgeLocation, HasRoomCheck_BackLedgeLocation, CachedCapsuleRadius, CachedCapsuleHalfHeight))
		{
			// 장애물까지의 깊이 구하기
			OutParkourCheckResult.bHasBackLedge = false;
			OutParkourCheckResult.ObstacleDepth = FVector::Dist2D(HitResult.ImpactPoint, OutParkourCheckResult.FrontLedgeLocation);
		}
		else
		{
			// 난간 깊이 구하기
			OutParkourCheckResult.bHasBackLedge = true;
			OutParkourCheckResult.ObstacleDepth = FVector::Dist2D(OutParkourCheckResult.FrontLedgeLocation, OutParkourCheckResult.BackLedgeLocation);
		}

		if (OutParkourCheckResult.bHasBackLedge)
		{
			// 난간 뒤 공간 확인
			const float BackLedgeVerticalOffsetZ = OutParkourCheckResult.ObstacleHeight - InParkourCheckInputs.CapsuleHalfHeight + 50.f;

			FVector HasFloorCheck_BackLedgeLocation = OutParkourCheckResult.BackLedgeLocation + BackHorizontalOffset;
			HasFloorCheck_BackLedgeLocation.Z -= BackLedgeVerticalOffsetZ;

			if (GenerateCapsuleTrace(HitResult, HasRoomCheck_BackLedgeLocation, HasFloorCheck_BackLedgeLocation, CachedCapsuleRadius, CachedCapsuleHalfHeight))
			{
				// 난간 뒤 높이 구하기
				OutParkourCheckResult.bHasBackFloor = true;
				OutParkourCheckResult.BackFloorLocation = HitResult.ImpactPoint;
				OutParkourCheckResult.BackLedgeHeight = FMath::Abs(HitResult.ImpactPoint.Z - OutParkourCheckResult.BackLedgeLocation.Z);
			}
			else
			{
				OutParkourCheckResult.bHasBackFloor = false;
			}
		}
		
		if (!OutParkourCheckResult.bHasFrontLedge)
		{
			return false;
		}

		OutParkourCheckResult.ActionType = SelectParkourAction(OutParkourCheckResult.ObstacleHeight, OutParkourCheckResult.ObstacleDepth);
		if (OutParkourCheckResult.ActionType == EParkourActionType::None)
		{
			return false;
		}

		return DoParkourAction(OutParkourCheckResult, InParkourCheckInputs.CapsuleRadius);

	}
	
	return false;

}

void UParkourComponent::BeginPlay()
{
	Super::BeginPlay();
	
	//check(!AM_ParkourAnimMontage.IsEmpty());
}

EParkourActionType UParkourComponent::SelectParkourAction(const float Height, const float Depth)
{
	EParkourActionType ParkourAction = EParkourActionType::None;

	if (Depth < 75)
	{
		if (75 < Height && Height < 100)
		{
			ParkourAction = EParkourActionType::Hurdle;
		}
		else if (Height < 120)
		{
			ParkourAction = EParkourActionType::Vault;
		}
		else if (Height < 150)
		{
			ParkourAction = EParkourActionType::Mantle;
		}
		else
		{
			return ParkourAction;
		}
	}
	else
	{
		ParkourAction = EParkourActionType::Mantle;

	}

	return ParkourAction;
}

bool UParkourComponent::DoParkourAction(const FParkourCheckResult& Input, const float Offset)
{
	EParkourActionType ParkourAction = Input.ActionType;

	if (ParkourAction == EParkourActionType::None)
	{
		return false;
	}

	bDoingParkourAction = true;
	CharacterProperties.Capsule->IgnoreActorWhenMoving(CachedTarget, true);
	CharacterProperties.Movement->SetMovementMode(EMovementMode::MOVE_Flying);

	// 시작 지점 모션 워핑
	const FName FrontLedge = TEXT("FrontLedge");
	const FVector FrontLocationOffset = Input.FrontLedgeNormal * Offset;
	const FVector FrontLocation = Input.FrontLedgeLocation + FrontLocationOffset;
	const FVector NegateNormal = Input.FrontLedgeNormal * -1.f;
	const FRotator DesiredActorRotation = FRotationMatrix::MakeFromX(NegateNormal).Rotator();
	
	CharacterProperties.MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(FrontLedge, FrontLocation, DesiredActorRotation);

	// 건너편 모션 워핑
	const FName BackLedge = TEXT("BackLedge");
	const FVector BackLocationOffset = Input.BackLedgeNormal * Offset;
	const FVector BackLocation = Input.BackLedgeLocation + BackLocationOffset;

	CharacterProperties.MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(BackLedge, BackLocation, DesiredActorRotation);

	UAnimMontage* AMParkour = AM_ParkourAnimMontage.FindRef(ParkourAction);
	if (IsValid(AMParkour))
	{
		CharacterProperties.Anim->Montage_Play(AMParkour);
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &UParkourComponent::OnParkourActionEnded);
		CharacterProperties.Anim->Montage_SetEndDelegate(EndDelegate, AMParkour);

		return true;
	}

	UE_LOG(LogTemp, Error, TEXT("FrontLocation : %s"), *FrontLocation.ToString());
	UE_LOG(LogTemp, Error, TEXT("BackLocation : %s"), *BackLocation.ToString());
	return false;

}

bool UParkourComponent::GenerateCapsuleTrace(FHitResult& OutHitResult, const FVector& InTraceStart, const FVector& InTraceEnd, float InCapsuleRadius, float InCapsuleHalfHeight, bool bFindParkourableActor)
{
	OutHitResult.Reset();

	FCollisionShape Shape = FCollisionShape::MakeCapsule(InCapsuleRadius, InCapsuleHalfHeight);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	bool bHit = GetWorld()->SweepSingleByChannel(
		OutHitResult,
		InTraceStart,
		InTraceEnd,
		FQuat::Identity,
		bFindParkourableActor ? ECC_Parkourable : ECC_Visibility, // 파쿠르 액터 or 장애물 감지
		Shape,
		Params
	);

	return bHit;
}

void UParkourComponent::OnParkourActionEnded(UAnimMontage* InMontage, bool bInterrupted)
{
	bDoingParkourAction = false;
	CharacterProperties.Capsule->IgnoreActorWhenMoving(CachedTarget, false);
	CharacterProperties.Movement->SetMovementMode(EMovementMode::MOVE_Walking);
	CachedTarget = nullptr;
	CharacterProperties.Anim->OnMontageEnded.RemoveDynamic(this, &UParkourComponent::OnParkourActionEnded);
}
