#include "AI/BTTask_Attack.h"
#include "Controller/KAIController.h"
#include "Character/KNonPlayerCharacter.h"
#include "Components/KStatusComponent.h"

UBTTask_Attack::UBTTask_Attack()
{
	bNotifyTick = true;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AKAIController* AIController = Cast<AKAIController>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("Invalid AIController."));

	AKNonPlayerCharacter* NPC = Cast<AKNonPlayerCharacter>(AIController->GetPawn());
	checkf(IsValid(NPC) == true, TEXT("Invalid NPC."));
	
	if (NPC->GetStatusComponent()->bIsActionLocked == false)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AKAIController* AIController = Cast<AKAIController>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("Invalid AIController."));

	AKNonPlayerCharacter* NPC = Cast<AKNonPlayerCharacter>(AIController->GetPawn());
	checkf(IsValid(NPC) == true, TEXT("Invalid NPC."));

	NPC->BeginAttack();

	return EBTNodeResult::InProgress;

}
