#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "KAIController.generated.h"

class UBlackboardData;
class UBehaviorTree;

UCLASS()
class KINET_API AKAIController : public AAIController
{
	GENERATED_BODY()

	friend class AKNonPlayerCharacter;

public:
	AKAIController();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void BeginAI(APawn* InPawn);
	void EndAI();

public:
	static const float PatrolRadius;
	static const FName StartPatrolPositionKey;
	static const FName EndPatrolPositionKey;
	static const FName TargetCharacterKey;
	static int32 ShowAIDebug;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UBlackboardData> BlackboardDataAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UBehaviorTree> BehaviorTree;

};
