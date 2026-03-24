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
	CharacterProperties = InCharacterProperties;
}

bool UParkourComponent::TryParkourAction(const FParkourCheckInputs& InParkourCheckInputs)
{
	if (!IsValid(CharacterProperties.MotionWarping))
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterProperties is invalid."));
		return false;
	}

	if (AActor* OwnerActor = GetOwner())
	{
		// 파쿠르 가능한 액터 체크
		FHitResult HitResult;
		const FVector ActorLocation = OwnerActor->GetActorLocation();
		const FVector TraceEndLocation = ActorLocation + (InParkourCheckInputs.TraceForwardDirection * InParkourCheckInputs.TraceForwardDistance);
		const float CapsuleRadius = InParkourCheckInputs.TraceRadius;
		const float CapsuleHalfHeight = InParkourCheckInputs.TraceHalfHeight;
		const FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(OwnerActor);

		bool bHit = GetWorld()->SweepSingleByChannel(
			HitResult,
			ActorLocation,
			TraceEndLocation,
			FQuat::Identity,
			ECC_Parkourable,
			CapsuleShape,
			Params
		);

		if (bHit == false) return false;

		if (AParkourableActor* ParkourableActor = Cast<AParkourableActor>(HitResult.GetActor()))
		{
			ParkourResult.Clear();
			ParkourableActor->GetLedgeTransforms(HitResult.ImpactPoint, ActorLocation, ParkourResult);
			
			if (ParkourResult.bHasFrontLedge == false) return false;
		}

	}

	// 장애물 감지
	const FVector ActorLocation = GetOwner()->GetActorLocation();
	const float CapsuleRadius = CharacterProperties.Capsule->GetScaledCapsuleRadius();
	const float CapsuleHalfHeight = CharacterProperties.Capsule->GetScaledCapsuleHalfHeight();
	const FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	const float Offset = 2.0f;
	const FVector HorizontalOffset = ParkourResult.FrontLedgeNormal * (CapsuleRadius + Offset);
	const float VerticalOffsetZ = CapsuleHalfHeight + Offset;

	FVector HasRoomCheck_FrontLedgeLocation = ParkourResult.FrontLedgeLocation + HorizontalOffset;
	HasRoomCheck_FrontLedgeLocation.Z += VerticalOffsetZ;
	
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		ActorLocation,
		HasRoomCheck_FrontLedgeLocation,
		FQuat::Identity,
		ECC_Visibility,
		CapsuleShape,
		Params
	);

	if (bHit == false)
	{
		// Step 3.3 높이 구하기
	}
	else return false;

	return false;

}

void UParkourComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


