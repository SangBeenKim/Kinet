#include "AI/BTDecorator_IsInAttackRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/KAIController.h"
#include "Character/KNonPlayerCharacter.h"

const float UBTDecorator_IsInAttackRange::AttackRange(200.f);

UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
	NodeName = TEXT("IsInAttackRange");
}

bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	checkf(bResult == true, TEXT("Super::CalculateRawConditionValue() function has returned false."));

	AKAIController* AIController = Cast<AKAIController>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("Invalid AIController."));

	AKNonPlayerCharacter* NPC = Cast<AKNonPlayerCharacter>(AIController->GetPawn());
	checkf(IsValid(NPC) == true, TEXT("Invalid NPC."));

	AKCharacterBase* TargetPlayerCharacter = Cast<AKCharacterBase>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AKAIController::TargetCharacterKey));
	if (IsValid(TargetPlayerCharacter) && TargetPlayerCharacter->IsPlayerControlled())
	{
		return NPC->GetDistanceTo(TargetPlayerCharacter) <= AttackRange;
	}

	return false;
}
