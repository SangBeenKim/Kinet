#include "UI/ResultWidget.h"
#include "UI/MenuLayout.h"
#include "Components/TextBlock.h"

void UResultWidget::CreateResult(const FText& InResultText, const TArray<FMenuButtonData>& MenuDataList)
{
	ResultText->SetText(InResultText);
	ResultMenu->BuildMenu(MenuDataList);
}

void UResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true;

	if (IsValid(ResultMenu))
	{
		if (!ResultMenu->OnMenuActionTriggerd.IsAlreadyBound(this, &ThisClass::SelectMenu))
		{
			ResultMenu->OnMenuActionTriggerd.AddDynamic(this, &ThisClass::SelectMenu);
		}
	}

}

void UResultWidget::SelectMenu(const FName& InActionID)
{
	OnMenuActionSignature.Broadcast(InActionID);
}