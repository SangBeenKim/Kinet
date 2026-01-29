#include "UI/HPBar.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Character/KCharacterBase.h"

void UHPBar::InitializeHPBarWidget(AKCharacterBase* InCharacter)
{
	OnMaxHPChange(InCharacter->GetMaxHP());
	InCharacter->OnMaxHPChanged.AddUObject(this, &ThisClass::OnMaxHPChange);
	OnCurrentHPChange(InCharacter->GetCurrentHP());
	InCharacter->OnCurrentHPChanged.AddUObject(this, &ThisClass::OnCurrentHPChange);
}

void UHPBar::UpdateProgressBar()
{
	if (IsValid(HPProgressBar) == true)
	{
		if (KINDA_SMALL_NUMBER < LastUpdatedMaxHP)
		{
			HPProgressBar->SetPercent(LastUpdatedCurrentHP / LastUpdatedMaxHP);
		}
		else
		{
			HPProgressBar->SetPercent(0.f);
		}
	}
}

void UHPBar::OnMaxHPChange(float InMaxHP)
{
	if (LastUpdatedMaxHP == InMaxHP)
	{
		return;
	}

	LastUpdatedMaxHP = InMaxHP;

	MaxHPText->SetText(FText::FromString(FString::SanitizeFloat(InMaxHP)));

	UpdateProgressBar();
}

void UHPBar::OnCurrentHPChange(float InCurrentHP)
{
	if (LastUpdatedCurrentHP == InCurrentHP)
	{
		return;
	}

	LastUpdatedCurrentHP = InCurrentHP;

	CurrentHPText->SetText(FText::FromString(FString::SanitizeFloat(InCurrentHP)));

	UpdateProgressBar();
}
