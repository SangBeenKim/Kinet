#include "UI/KHUD.h"
#include "UI/HPBar.h"
#include "UI/PauseMenu.h"
#include "Components/Image.h"

void UKHUD::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(HUD_HPBar))
	{
		HUD_HPBar->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (IsValid(HUD_PauseMenu))
	{
		HUD_PauseMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (IsValid(Crosshair))
	{
		Crosshair->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UKHUD::InitializeHUD(IHealthInterface* InOwner)
{
	if (IsValid(HUD_HPBar))
	{
		HUD_HPBar->InitializeHPBarWidget(InOwner);
		HUD_HPBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void UKHUD::SetPauseMenuVisible(bool bVisible)
{
	if (!IsValid(HUD_PauseMenu)) return;

	if (bVisible)
	{
		HUD_PauseMenu->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		HUD_PauseMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UKHUD::SetCrosshairVisible(bool bVisible)
{
	if (!IsValid(Crosshair)) return;

	if (bVisible)
	{
		Crosshair->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Crosshair->SetVisibility(ESlateVisibility::Collapsed);
	}
}
