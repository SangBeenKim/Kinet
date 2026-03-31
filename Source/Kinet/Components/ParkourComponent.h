#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ParkourComponent.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UCharacterMovementComponent;
class UMotionWarpingComponent;

UENUM()
enum class EParkourActionType : uint8
{
	None,
	Hurdle,
	Vault,
	Mantle,
};

struct FParkourCheckResult
{
	FParkourCheckResult()
		: ActionType(EParkourActionType::None)
		, bHasFrontLedge(false)
		, bHasBackLedge(false)
		, bHasBackFloor(false)
		, ObstacleHeight(0.f)
		, ObstacleDepth(0.f)
	{}

	EParkourActionType ActionType;
	bool bHasFrontLedge;
	FVector FrontLedgeLocation;
	FVector FrontLedgeNormal;
	bool bHasBackLedge;
	FVector BackLedgeLocation;
	FVector BackLedgeNormal;
	bool bHasBackFloor;
	FVector BackFloorLocation;
	float ObstacleHeight;
	float ObstacleDepth;
	float BackLedgeHeight;
};

struct FParkourCheckInputs
{
	FParkourCheckInputs()
		: TraceForwardDistance(75.f)
		, CapsuleRadius(30.f)
		, CapsuleHalfHeight(60.f)
	{}

	FVector TraceForwardDirection;
	FVector ActorLocation;
	float TraceForwardDistance;
	float CapsuleRadius;
	float CapsuleHalfHeight;
};

USTRUCT(BlueprintType)
struct FCharacterPropertiesForParkour
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UCapsuleComponent> Capsule;
	UPROPERTY()
	TObjectPtr<UAnimInstance> Anim;
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> Movement;
	UPROPERTY()
	TObjectPtr<UMotionWarpingComponent> MotionWarping;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KINET_API UParkourComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UParkourComponent();
	void SetCharacterProperties(const FCharacterPropertiesForParkour& InCharacterProperties);
	bool TryParkourAction(const FParkourCheckInputs& InParkourCheckInputs, FParkourCheckResult& OutParkourCheckResult);
	bool DoingParkourAction() { return bDoingParkourAction; }

protected:
	virtual void BeginPlay() override;

private:
	EParkourActionType SelectParkourAction(const float Height, const float Depth);
	bool DoParkourAction(const FParkourCheckResult& Input, const float Offset);
	bool GenerateCapsuleTrace(FHitResult& OutHitResult, const FVector& InTraceStart, const FVector& InTraceEnd, float InCapsuleRadius, float InCapsuleHalfHeight, bool bFindParkourableActor = false);
	void OnParkourActionEnded(UAnimMontage* InMontage, bool bInterrupted);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "ParkourAnimation")
	TMap<EParkourActionType, TObjectPtr<UAnimMontage>> AM_ParkourAnimMontage;

private:
	UPROPERTY()
	FCharacterPropertiesForParkour CharacterProperties;
	UPROPERTY()
	TObjectPtr<AActor> CachedTarget;
	bool bDoingParkourAction;
};
