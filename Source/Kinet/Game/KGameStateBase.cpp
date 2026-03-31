#include "Game/KGameStateBase.h"
#include "UI/ResultWidget.h"
#include "Data/MenuButtonData.h"
#include "Game/KGameModeBase.h"
#include "Controller/KPlayerController.h"

void AKGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	AKGameModeBase* GM = Cast<AKGameModeBase>(GetWorld()->GetAuthGameMode());
	if (IsValid(GM))
	{
		GM->OnCurrentGameStateChanged.RemoveAll(this);
		GM->OnCurrentGameStateChanged.AddUObject(this, &ThisClass::CreateResultWidget);
	}
}

void AKGameStateBase::CreateResultWidget(EGameFlowState InCurrentGameState)
{
	if (IsValid(ResultWidget)) return;

	CurrentGameState = InCurrentGameState;

	if (IsValid(ResultWidgetClass))
	{
		AKPlayerController* PC = Cast<AKPlayerController>(GetWorld()->GetFirstPlayerController());
		if (IsValid(PC))
		{
			ResultWidget = CreateWidget<UResultWidget>(PC, ResultWidgetClass);
			if (IsValid(ResultWidget))
			{
				CreateResultMenu(PC);
				ResultWidget->AddToViewport();

				FInputModeUIOnly InputModeUIOnly;
				PC->SetInputMode(InputModeUIOnly);
				PC->bShowMouseCursor = true;
			}
		}
		
	}
}

void AKGameStateBase::CreateResultMenu(AKPlayerController* InPlayerController)
{
	if (!IsValid(ResultWidget)) return;

	if (!IsValid(InPlayerController)) return;

	if (CurrentGameState == EGameFlowState::GameOver)
	{
		FText ResultText = FText::FromString(TEXT("Game Over"));
		TArray<FMenuButtonData> MenuList;

		MenuList.Emplace(FName(TEXT("Start")), FText::FromString(TEXT("Retry")));
		MenuList.Emplace(FName(TEXT("Title")), FText::FromString(TEXT("Return Title")));

		ResultWidget->CreateResult(ResultText, MenuList);
		if (!ResultWidget->OnMenuActionSignature.IsAlreadyBound(InPlayerController, &AKPlayerController::HandleMenuAction))
		{
			ResultWidget->OnMenuActionSignature.AddDynamic(InPlayerController, &AKPlayerController::HandleMenuAction);
		}

	}
	else if (CurrentGameState == EGameFlowState::GameClear)
	{
		FText ResultText = FText::FromString(TEXT("Game Clear"));
		TArray<FMenuButtonData> MenuList;

		MenuList.Emplace(FName(TEXT("Start")), FText::FromString(TEXT("Retry")));
		MenuList.Emplace(FName(TEXT("Title")), FText::FromString(TEXT("Return Title")));

		ResultWidget->CreateResult(ResultText, MenuList);
		if (!ResultWidget->OnMenuActionSignature.IsAlreadyBound(InPlayerController, &AKPlayerController::HandleMenuAction))
		{
			ResultWidget->OnMenuActionSignature.AddDynamic(InPlayerController, &AKPlayerController::HandleMenuAction);
		}
	}
	else
	{
		FString StateName = StaticEnum<EGameFlowState>()->GetValueAsString(CurrentGameState);
		UE_LOG(LogTemp, Warning, TEXT("Unhandled Game State: %s"), *StateName);
	}

}
