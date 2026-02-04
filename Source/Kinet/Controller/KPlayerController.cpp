#include "Controller/KPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Game/KGameInstance.h"
#include "UI/PauseMenu.h" // <- 의존성 분리 예정
#include "UI/KHUD.h"
#include "Character/KCharacterBase.h"
#include "Components/KStatusComponent.h"

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

	CreateHUD();

}

void AKPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (IsValid(EIC))
	{
		if (IsValid(IA_Pause))
		{
			IA_Pause->bTriggerWhenPaused = true;
			EIC->BindAction(IA_Pause, ETriggerEvent::Started, this, &ThisClass::TogglePauseMenu);
		}
	}

}

void AKPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	CreateHUD();

	AKCharacterBase* PlayerCharacter = Cast<AKCharacterBase>(InPawn);
	if (IsValid(PlayerCharacter))
	{
		if (IsValid(HUDInstance))
		{
			HUDInstance->InitializeHUD(PlayerCharacter->GetStatusComponent());
		}
	}
}

void AKPlayerController::TogglePauseMenu()
{
	bool bNewPauseState = !IsPaused();

	SetPause(bNewPauseState);
	
	if (IsValid(HUDInstance))
	{
		bNewPauseState ? HUDInstance->ShowPauseMenu() : HUDInstance->HidePauseMenu();

		if (bNewPauseState)
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(HUDInstance->GetCachedWidget());
			SetInputMode(InputMode);
			bShowMouseCursor = true;
		}
		else
		{
			SetInputMode(FInputModeGameOnly());
			bShowMouseCursor = false;
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

void AKPlayerController::CreateHUD()
{
	if (!ensureMsgf(IsValid(HUDClass), TEXT("HUDClass is invalid.")))
	{
		return;
	}

	if (IsValid(HUDInstance))
	{
		return;
	}

	HUDInstance = CreateWidget<UKHUD>(this, HUDClass);
	if (IsValid(HUDInstance))
	{
		HUDInstance->HUD_PauseMenu->OnSelectedMenu.AddUniqueDynamic(this, &ThisClass::HandleMenuAction);
		HUDInstance->AddToViewport();

		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;
	}
}
