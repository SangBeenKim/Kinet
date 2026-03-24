#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ParkourComponent.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UMotionWarpingComponent;

UENUM()
enum class EParkourActionType : uint8
{
	None,
	Hurdle,
	Vault,
	Mantle,
};

USTRUCT(BlueprintType)
struct KINET_API FParkourCheckResult
{
	GENERATED_BODY()
	FParkourCheckResult()
		: ActionType(EParkourActionType::None)
		, bHasFrontLedge(false)
		, bHasBackLedge(false)
		, bHasBackFloor(false)
		, ObstacleHeight(0.f)
		, ObstacleDepth(0.f)
	{}

	void Clear()
	{
		ActionType = EParkourActionType::None;
		bHasFrontLedge = false;
		FrontLedgeLocation = FVector::ZeroVector;
		FrontLedgeNormal = FVector::ZeroVector;
		bHasBackLedge = false;
		BackLedgeLocation = FVector::ZeroVector;
		BackLedgeNormal = FVector::ZeroVector;
		bHasBackFloor = false;
		BackFloorLocation = FVector::ZeroVector;
		ObstacleHeight = 0.f;
		ObstacleDepth = 0.f;
	}

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
};

USTRUCT(BlueprintType)
struct KINET_API FParkourCheckInputs
{
	GENERATED_BODY()
	FParkourCheckInputs()
		: TraceForwardDistance(75.f)
		, TraceRadius(30.f)
		, TraceHalfHeight(60.f)
	{}

	FVector TraceForwardDirection;
	float TraceForwardDistance;
	float TraceRadius;
	float TraceHalfHeight;
};

USTRUCT(BlueprintType)
struct KINET_API FCharacterPropertiesForParkour
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UCapsuleComponent> Capsule;
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> Mesh;
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
	bool TryParkourAction(const FParkourCheckInputs& InParkourCheckInputs);

protected:
	virtual void BeginPlay() override;

protected:
	FParkourCheckResult ParkourResult;
	FCharacterPropertiesForParkour CharacterProperties;

};
