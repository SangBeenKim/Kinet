#include "UI/MenuLayout.h"
#include "Components/VerticalBox.h"
#include "UI/MenuButton.h"

void UMenuLayout::BuildMenu(const TArray<FMenuButtonData>& MenuDataList)
{
	if (!IsValid(ButtonList) || !IsValid(MenuButtonClass)) return;

	ButtonList->ClearChildren();

	for (const FMenuButtonData& Data : MenuDataList)
	{
		UMenuButton* NewButton = CreateWidget<UMenuButton>(this, MenuButtonClass);
		if (IsValid(NewButton))
		{
			NewButton->InitButton(Data);
			if (!NewButton->OnButtonClicked.IsAlreadyBound(this, &ThisClass::HandleButtonAction))
			{
				NewButton->OnButtonClicked.AddDynamic(this, &ThisClass::HandleButtonAction);
			}
			ButtonList->AddChild(NewButton);
		}
	}

}

void UMenuLayout::HandleButtonAction(const FName& InActionID)
{
	OnMenuActionTriggerd.Broadcast(InActionID);
}
