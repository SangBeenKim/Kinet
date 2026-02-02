#include "Controller/KAIController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h" // LOG

const float AKAIController::PatrolRadius(500.f);
int32 AKAIController::ShowAIDebug(0);
const FName AKAIController::StartPatrolPositionKey(TEXT("StartPatrolPosition"));
const FName AKAIController::EndPatrolPositionKey(TEXT("EndPatrolPosition"));
const FName AKAIController::TargetCharacterKey(TEXT("TargetCharacter"));

FAutoConsoleVariableRef CVarShowAIDebug(
	TEXT("Kinet.ShowAIDebug"),
	AKAIController::ShowAIDebug,
	TEXT(""),
	ECVF_Cheat
);

AKAIController::AKAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BrainComponent"));
}

void AKAIController::BeginPlay()
{
	Super::BeginPlay();

	APawn* ControlledPawn = GetPawn();
	if (IsValid(ControlledPawn))
	{
		BeginAI(ControlledPawn);
	}

}

void AKAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndAI();

	Super::EndPlay(EndPlayReason);
}

void AKAIController::BeginAI(APawn* InPawn)
{
	UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(Blackboard);
	if (IsValid(BlackboardComponent))
	{
		if (UseBlackboard(BlackboardDataAsset, BlackboardComponent) == true)
		{
			bool bRunSucceeded = RunBehaviorTree(BehaviorTree);
			checkf(bRunSucceeded == true, TEXT("Fail to run behavior tree."));

			BlackboardComponent->SetValueAsVector(StartPatrolPositionKey, InPawn->GetActorLocation());

			if (ShowAIDebug == 1)
			{
				UKismetSystemLibrary::PrintString(
					this,
					FString(TEXT("BeginAI()"))
				);
			}

		}
	}
}

void AKAIController::EndAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (IsValid(BehaviorTreeComponent))
	{
		BehaviorTreeComponent->StopTree();

		if (ShowAIDebug == 1)
		{
			UKismetSystemLibrary::PrintString(
				this,
				FString::Printf(TEXT("EndAI()"))
			);
		}
	}

}
