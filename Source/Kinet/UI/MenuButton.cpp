#include "UI/MenuButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

UMenuButton::UMenuButton(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, AssignedActionID(TEXT("None"))
{
}

void UMenuButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (!ensureMsgf(IsValid(Button), TEXT("Button is invalid.")))
	{
		return;
	}

	if (!Button->OnClicked.IsAlreadyBound(this, &ThisClass::OnInternalButtonClicked))
	{
		Button->OnClicked.AddDynamic(this, &ThisClass::OnInternalButtonClicked);
	}

}

void UMenuButton::InitButton(const FMenuButtonData& InData)
{
	AssignedActionID = InData.ActionID;
	if (IsValid(ButtonText))
	{
		ButtonText->SetText(InData.ButtonLabel);
	}
}

void UMenuButton::OnInternalButtonClicked()
{
	OnButtonClicked.Broadcast(AssignedActionID);
}
