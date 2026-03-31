#include "AI/BTTask_GetEndPatrolPosition.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/KAIController.h"
#include "Character/KNonPlayerCharacter.h"

UBTTask_GetEndPatrolPosition::UBTTask_GetEndPatrolPosition()
{
	NodeName = TEXT("GetEndPatrolPosition");
}

EBTNodeResult::Type UBTTask_GetEndPatrolPosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if (EBTNodeResult::Failed == Result)
	{
		return Result;
	}

	AKAIController* AIController = Cast<AKAIController>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("Invalid AIController"));

	AKNonPlayerCharacter* NPC = Cast<AKNonPlayerCharacter>(AIController->GetPawn());
	checkf(IsValid(NPC) == true, TEXT("Invalid NPC"));

	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetNavigationSystem(NPC->GetWorld());
	checkf(IsValid(NavigationSystem) == true, TEXT("Invalid NavigationSystem"));

	FVector StartPatrolPosition = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AKAIController::StartPatrolPositionKey);
	FNavLocation EndPatrolLocation;
	if (true == NavigationSystem->GetRandomPointInNavigableRadius(StartPatrolPosition, AIController->PatrolRadius, EndPatrolLocation))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AKAIController::EndPatrolPositionKey, EndPatrolLocation.Location);
		return Result = EBTNodeResult::Succeeded;
	}

	return Result;

}
