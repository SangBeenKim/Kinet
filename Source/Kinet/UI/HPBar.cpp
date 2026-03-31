#include "UI/HPBar.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Interfaces/HealthInterface.h"

void UHPBar::InitializeHPBarWidget(IHealthInterface* InOwner)
{
	if (InOwner == nullptr) return;

	OwnerInterface = InOwner;
	Owner = Cast<UObject>(OwnerInterface);
	ensureMsgf(Owner != nullptr, TEXT("[%s] Casting failed IHealthInterface to UObject."), *this->GetName());
	
	UpdateProgressBar(OwnerInterface->GetCurrentHealth(), OwnerInterface->GetMaxHealth());
	OwnerInterface->GetOnUpdatedHealthDelegate().AddUniqueDynamic(this, &ThisClass::UpdateProgressBar);
}

void UHPBar::UpdateProgressBar(float InCurrentHP, float InMaxHP)
{
	if (IsValid(HPProgressBar) == true)
	{
		if (KINDA_SMALL_NUMBER < InMaxHP)
		{
			HPProgressBar->SetPercent(InCurrentHP / InMaxHP);
		}
		else
		{
			HPProgressBar->SetPercent(0.f);
		}
	}
}

void UHPBar::NativeDestruct()
{
	if (Owner.IsValid() && OwnerInterface != nullptr)
	{
		OwnerInterface->GetOnUpdatedHealthDelegate().RemoveAll(this);
	}

	OwnerInterface = nullptr;
	Owner.Reset();

	Super::NativeDestruct();
}
