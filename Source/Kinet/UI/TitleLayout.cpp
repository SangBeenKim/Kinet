#include "UI/TitleLayout.h"
#include "UI/MenuLayout.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Game/KGameInstance.h"

UTitleLayout::UTitleLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsAnyKeyPressed(false)
{
}

void UTitleLayout::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true;

	if (IsValid(PressAnyKeyText))
	{
		PressAnyKeyText->SetVisibility(ESlateVisibility::Visible);
		ensureMsgf(IsValid(PressAnyKeyTextAnim), TEXT("PressAnyKeyTextAnim is invalid."));
		if (IsValid(PressAnyKeyTextAnim))
		{
			PlayAnimation(PressAnyKeyTextAnim, 0.f, 0, EUMGSequencePlayMode::Forward, 1.f);	
		}
	}

	if (IsValid(TitleMenu))
	{
		if (!TitleMenu->OnMenuActionTriggerd.IsAlreadyBound(this, &ThisClass::SelectMenu))
		{
			TitleMenu->OnMenuActionTriggerd.AddDynamic(this, &ThisClass::SelectMenu);
		}

		TitleMenu->BuildMenu(TitleMenuData);
		TitleMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
	
}

FReply UTitleLayout::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (bIsAnyKeyPressed == true)
	{
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	}

	ShowMainMenu();
	return FReply::Handled();
}

FReply UTitleLayout::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsAnyKeyPressed == true)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	ShowMainMenu();
	return FReply::Handled();
}

void UTitleLayout::ShowMainMenu()
{
	bIsAnyKeyPressed = true;

	PressAnyKeyText->SetVisibility(ESlateVisibility::Collapsed);
	if (IsValid(PressAnyKeyTextAnim))
	{
		StopAnimation(PressAnyKeyTextAnim);
	}

	TitleMenu->SetVisibility(ESlateVisibility::Visible);
	
}

void UTitleLayout::SelectMenu(const FName& InActionID)
{
	UKGameInstance* GI = Cast<UKGameInstance>(GetGameInstance());
	checkf(IsValid(GI), TEXT("GameInstance is invalid."));
	GI->MoveToLevel(InActionID);
}
