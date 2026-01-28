#include "UI/PauseMenu.h"
#include "UI/MenuLayout.h"
#include "Kismet/KismetSystemLibrary.h" // LOG

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
	FName Value = InActionID;

	UKismetSystemLibrary::PrintString(
		this,
		Value.ToString()
	);
}
