#include "AI/BTService_DetectPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/OverlapResult.h"
#include "Controller/KAIController.h"
#include "Character/KNonPlayerCharacter.h"

UBTService_DetectPlayerCharacter::UBTService_DetectPlayerCharacter(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("DetectPlayerCharacter");
	Interval = 1.f;
}

void UBTService_DetectPlayerCharacter::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AKAIController* AIC = Cast<AKAIController>(OwnerComp.GetAIOwner());
	if (IsValid(AIC))
	{
		AKNonPlayerCharacter* NPC = Cast<AKNonPlayerCharacter>(AIC->GetPawn());
		if (IsValid(NPC))
		{
			UWorld* World = NPC->GetWorld();
			if (IsValid(World))
			{
				FVector CenterPosition = NPC->GetActorLocation();
				float DetectRadius = 300.f;
				TArray<FOverlapResult> OverlapResults;
				FCollisionQueryParams CollisionQueryParams(NAME_None, false, NPC);
				bool bResult = World->OverlapMultiByChannel(
					OverlapResults,
					CenterPosition,
					FQuat::Identity,
					ECollisionChannel::ECC_GameTraceChannel12,
					FCollisionShape::MakeSphere(DetectRadius),
					CollisionQueryParams
				);

				if (bResult == true)
				{
					for (auto const& OverlapResult : OverlapResults)
					{
						AKCharacterBase* PC = Cast<AKCharacterBase>(OverlapResult.GetActor());
						if (IsValid(PC) && PC->IsPlayerControlled() == true)
						{
							OwnerComp.GetBlackboardComponent()->SetValueAsObject(AKAIController::TargetCharacterKey, PC);

							if (AKAIController::bShowAIDebug)
							{
								UKismetSystemLibrary::PrintString(
									this,
									FString::Printf(TEXT("Detected!"))
								);
								DrawDebugSphere(World, CenterPosition, DetectRadius, 16, FColor::Red, false, 0.5f);
								DrawDebugPoint(World, PC->GetActorLocation(), 10.f, FColor::Red, false, 0.5f);
								DrawDebugLine(World, NPC->GetActorLocation(), PC->GetActorLocation(), FColor::Red, false, 0.5f, 0u, 3.f);
							}

							break;
						}
						else
						{
							OwnerComp.GetBlackboardComponent()->SetValueAsObject(AKAIController::TargetCharacterKey, nullptr);

							if (AKAIController::bShowAIDebug)
							{
								DrawDebugSphere(World, CenterPosition, DetectRadius, 16, FColor::Green, false, 0.5f);
							}
						}
					}
				}
				else
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(AKAIController::TargetCharacterKey, nullptr);
				}

				if (AKAIController::bShowAIDebug)
				{
					DrawDebugSphere(World, CenterPosition, DetectRadius, 16, FColor::Green, false, 0.5f);
				}
			}
		}
	}
}
