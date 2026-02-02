#include "UI/KHUD.h"
#include "UI/HPBar.h"
#include "UI/PauseMenu.h"

void UKHUD::NativeConstruct()
{
	if (IsValid(HUD_HPBar))
	{
		HUD_HPBar->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (IsValid(HUD_PauseMenu))
	{
		HUD_PauseMenu->SetVisibility(ESlateVisibility::Collapsed);
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

void UKHUD::ShowPauseMenu()
{
	if (IsValid(HUD_PauseMenu))
	{
		HUD_PauseMenu->SetVisibility(ESlateVisibility::Visible);
	}
}

void UKHUD::ClosePauseMenu()
{
	if (IsValid(HUD_PauseMenu))
	{
		HUD_PauseMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
}
