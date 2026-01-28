#include "UI/PauseMenu.h"
#include "UI/MenuLayout.h"

void UPauseMenu::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true;

	if (IsValid(PauseMenu))
	{
		if (!PauseMenu->OnMenuActionTriggerd.IsAlreadyBound(this, &ThisClass::SelectMenu))
		{
			PauseMenu->OnMenuActionTriggerd.AddDynamic(this, &ThisClass::SelectMenu);
		}

		PauseMenu->BuildMenu(PauseMenuData);
	}
}

void UPauseMenu::SelectMenu(const FName& InActionID)
{
	OnSelectedMenu.Broadcast(InActionID);
}
