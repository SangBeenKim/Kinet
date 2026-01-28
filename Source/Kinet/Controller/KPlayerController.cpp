#include "Controller/KPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "UI/PauseMenu.h"
#include "Game/KGameInstance.h"

void AKPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* EIMC = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	if (IsValid(EIMC))
	{
		if (IMC_Default != nullptr)
		{
			EIMC->AddMappingContext(IMC_Default, 0);
		}
	}
}

void AKPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (IsValid(EIC))
	{
		if (IA_Pause != nullptr)
		{
			IA_Pause->bTriggerWhenPaused = true;
			EIC->BindAction(IA_Pause, ETriggerEvent::Started, this, &ThisClass::TogglePauseMenu);
		}
	}

}

void AKPlayerController::TogglePauseMenu()
{
	bool bNewPauseState = !IsPaused();

	SetPause(bNewPauseState);
	CheckPauseMenuWidget();
	
	if (IsValid(PauseMenuInstance))
	{
		PauseMenuInstance->SetVisibility(bNewPauseState ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (bNewPauseState)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(PauseMenuInstance->GetCachedWidget());
		SetInputMode(InputMode);
		bShowMouseCursor = true;
	}
	else
	{
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}

}

void AKPlayerController::CheckPauseMenuWidget()
{
	if (!ensureMsgf(IsValid(PauseMenuClass), TEXT("PauseMenuClass is invalid.")))
	{
		return;
	}
	
	if (PauseMenuInstance == nullptr)
	{
		PauseMenuInstance = CreateWidget<UPauseMenu>(this, PauseMenuClass);
		PauseMenuInstance->AddToViewport();
		PauseMenuInstance->SetVisibility(ESlateVisibility::Collapsed);
		if (!PauseMenuInstance->OnSelectedMenu.IsAlreadyBound(this, &ThisClass::HandleMenuAction))
		{
			PauseMenuInstance->OnSelectedMenu.AddDynamic(this, &ThisClass::HandleMenuAction);
		}
	}
}

void AKPlayerController::HandleMenuAction(const FName& InActionID)
{
	if (InActionID == "Resume")
	{
		TogglePauseMenu();
		return;
	}

	UKGameInstance* GI = GetGameInstance<UKGameInstance>();
	checkf(IsValid(GI), TEXT("UKGameInstance is invalid."));
	GI->MoveToLevel(InActionID);

}
