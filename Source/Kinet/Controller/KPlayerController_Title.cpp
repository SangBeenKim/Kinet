#include "Controller/KPlayerController_Title.h"
#include "UI/TitleLayout.h"

void AKPlayerController_Title::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(TitleWidgetClass))
	{
		TitleWidget = CreateWidget<UTitleLayout>(this, TitleWidgetClass);
		if (IsValid(TitleWidget))
		{
			TitleWidget->AddToViewport();
			TitleWidget->SetFocus();

			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetHideCursorDuringCapture(false);

			SetInputMode(InputMode);

			bShowMouseCursor = true;
		}
	}
}
