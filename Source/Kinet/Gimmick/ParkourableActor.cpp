#include "Gimmick/ParkourableActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/ParkourComponent.h"

AParkourableActor::AParkourableActor()
	: MinLedgeWidth(60.f)
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(SceneComp);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(SceneComp);

	for (int32 i = 0; i < 4; ++i)
	{
		FName LedgeName = *FString::Printf(TEXT("Ledge_%d"), i + 1);
		Ledges.Add(CreateDefaultSubobject<USplineComponent>(LedgeName));
		Ledges[i]->SetupAttachment(StaticMesh);
		Ledges[i]->EditorUnselectedSplineSegmentColor = FLinearColor::MakeFromHSV8(i * 70, 200, 200);
		Ledges[i]->EditorSelectedSplineSegmentColor = FLinearColor::MakeFromHSV8(i * 30, 200, 200);
	}

	for (int32 i = 0; i < Ledges.Num(); i += 2)
	{
		if (Ledges.IsValidIndex(i) && Ledges.IsValidIndex(i + 1))
		{
			OppositeLedges.Add(Ledges[i], Ledges[i + 1]);
			OppositeLedges.Add(Ledges[i + 1], Ledges[i]);
		}
	}

}

void AParkourableActor::GetLedgeTransforms(const FVector& InHitLocation, const FVector& InActorLocation, FParkourCheckResult& OutParkourResult)
{
	USplineComponent* ClosestLedge = FindLedgeClosestToActor(InActorLocation);
	if (!IsValid(ClosestLedge) || ClosestLedge->GetSplineLength() < MinLedgeWidth)
	{
		OutParkourResult.bHasFrontLedge = false;
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("ClosestLedge: %s"), *ClosestLedge->GetName());
	
	// 가까운 난간 좌표 구하기
	const FVector ClosestLocalLocation = ClosestLedge->FindLocationClosestToWorldLocation(InHitLocation, ESplineCoordinateSpace::Local);
	const float DistanceAlongSpline = ClosestLedge->GetDistanceAlongSplineAtLocation(ClosestLocalLocation, ESplineCoordinateSpace::Local);
	const float EdgePadding = MinLedgeWidth / 2.0f;
	const float MaxSplineLength = ClosestLedge->GetSplineLength();
	const float ClampedDistance = FMath::Clamp(DistanceAlongSpline, EdgePadding, MaxSplineLength - EdgePadding);
	const FTransform LedgeTransform = ClosestLedge->GetTransformAtDistanceAlongSpline(ClampedDistance, ESplineCoordinateSpace::World);

	OutParkourResult.bHasFrontLedge = true;
	OutParkourResult.FrontLedgeLocation = LedgeTransform.GetLocation();
	OutParkourResult.FrontLedgeNormal = LedgeTransform.Rotator().Quaternion().GetUpVector();

	// 반대쪽 난간 좌표 구하기
	USplineComponent* OppositeLedge = OppositeLedges.FindRef(ClosestLedge);
	if (!IsValid(OppositeLedge))
	{
		OutParkourResult.bHasBackLedge = false;
		return;
	}

	const FVector FrontLedgeLocation = OutParkourResult.FrontLedgeLocation;
	const FTransform OppositeLedgeTransform = OppositeLedge->FindTransformClosestToWorldLocation(FrontLedgeLocation, ESplineCoordinateSpace::World);
	
	OutParkourResult.bHasBackLedge = true;
	OutParkourResult.BackLedgeLocation = OppositeLedgeTransform.GetLocation();
	OutParkourResult.BackLedgeNormal = OppositeLedgeTransform.Rotator().Quaternion().GetUpVector();

}

void AParkourableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

USplineComponent* AParkourableActor::FindLedgeClosestToActor(const FVector& InActorLocation)
{
	if (Ledges.IsEmpty()) return nullptr;

	USplineComponent* ClosestLedge = nullptr;
	float MinDistSquared = TNumericLimits<float>::Max();

	for (USplineComponent* SplineComp : Ledges)
	{
		const FVector LedgeLocation = SplineComp->FindLocationClosestToWorldLocation(InActorLocation, ESplineCoordinateSpace::World);
		const FVector LedgeUpVector = SplineComp->FindUpVectorClosestToWorldLocation(InActorLocation, ESplineCoordinateSpace::World);
		const float Offset = 10.f;
		const FVector LedgeLocationOffset = LedgeLocation + (LedgeUpVector * Offset);
		const float CurrentDistSquared = FVector::DistSquared(LedgeLocationOffset, InActorLocation);

		if (CurrentDistSquared < MinDistSquared)
		{
			ClosestLedge = SplineComp;
			MinDistSquared = CurrentDistSquared;
		}
	}

	return ClosestLedge;
}
