#include "Components/ParkourComponent.h"
#include "Components/CapsuleComponent.h"
#include "MotionWarpingComponent.h"
#include "Gimmick/ParkourableActor.h"
#include "Kinet.h"

UParkourComponent::UParkourComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UParkourComponent::SetCharacterProperties(const FCharacterPropertiesForParkour& InCharacterProperties)
{
	//CharacterProperties = InCharacterProperties;
}

bool UParkourComponent::TryParkourAction(const FParkourCheckInputs& InParkourCheckInputs, FParkourCheckResult& OutParkourCheckResult)
{
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

		if (AParkourableActor* ParkourableActor = CastChecked<AParkourableActor>(HitResult.GetActor()))
		{
			if (!ParkourableActor->GetLedgeTransforms(HitResult.ImpactPoint, InParkourCheckInputs.ActorLocation, OutParkourCheckResult))
			{
				UE_LOG(LogTemp, Warning, TEXT("ParkourableActor Ledge is invalid."));
				return false; // 예외 처리
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
			return false; // 장애물 감지시 함수 종료
		}

		// 난간 높이 구하기
		const float GroundHeightFromActor = InParkourCheckInputs.ActorLocation.Z - InParkourCheckInputs.CapsuleHalfHeight;
		const float LedgeHeight = FMath::Abs(GroundHeightFromActor - OutParkourCheckResult.FrontLedgeLocation.Z);

		OutParkourCheckResult.ObstacleHeight = LedgeHeight;
		UE_LOG(LogTemp, Log, TEXT("ObstacleHeight : %f"), OutParkourCheckResult.ObstacleHeight);

		// 난간 위 장애물 감지
		const FVector BackHorizontalOffset = OutParkourCheckResult.BackLedgeNormal * (InParkourCheckInputs.CapsuleRadius + Offset);
		
		FVector HasRoomCheck_BackLedgeLocation = OutParkourCheckResult.BackLedgeLocation + BackHorizontalOffset;
		HasRoomCheck_BackLedgeLocation.Z += VerticalOffsetZ;

		if (GenerateCapsuleTrace(HitResult, HasRoomCheck_FrontLedgeLocation, HasRoomCheck_BackLedgeLocation, CachedCapsuleRadius, CachedCapsuleHalfHeight))
		{
			// 장애물까지의 깊이 구하기
			OutParkourCheckResult.bHasBackLedge = false;
			OutParkourCheckResult.ObstacleDepth = FVector::Dist2D(HitResult.ImpactPoint, OutParkourCheckResult.FrontLedgeLocation);
			UE_LOG(LogTemp, Log, TEXT("bHasBackLedge : false / ObstacleDepth : %f"), OutParkourCheckResult.ObstacleDepth);
		}
		else
		{
			// 난간 깊이 구하기
			OutParkourCheckResult.bHasBackLedge = true;
			OutParkourCheckResult.ObstacleDepth = FVector::Dist2D(OutParkourCheckResult.FrontLedgeLocation, OutParkourCheckResult.BackLedgeLocation);
			UE_LOG(LogTemp, Log, TEXT("bHasBackLedge : true / ObstacleDepth : %f"), OutParkourCheckResult.ObstacleDepth);
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
				UE_LOG(LogTemp, Log, TEXT("BackLedgeHeight : %f"), OutParkourCheckResult.BackLedgeHeight);
			}
			else
			{
				OutParkourCheckResult.bHasBackFloor = false;
				UE_LOG(LogTemp, Log, TEXT("bHasBackFloor : false"));
			}
		}
	}
	// 몽타주 선택을 위해 ActionType 결정, 모션 워핑 적용, 몽타주 재생
	return true;

}

void UParkourComponent::BeginPlay()
{
	Super::BeginPlay();
	
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
