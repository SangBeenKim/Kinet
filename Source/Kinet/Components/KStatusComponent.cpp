#include "Components/KStatusComponent.h"

UKStatusComponent::UKStatusComponent()
	: MaxHP(100.f)
	, CurrentHP(100.f)
	, bIsDead(false)
{
	PrimaryComponentTick.bCanEverTick = false;

}

float UKStatusComponent::ApplyDamage(float InDamage)
{
	const float PreviousHP = CurrentHP;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, PreviousHP);

	SetCurrentHP(PreviousHP - ActualDamage);

	return ActualDamage;
}

void UKStatusComponent::SetCurrentHP(float InCurrentHP)
{
	if (bIsDead) return;

	CurrentHP = InCurrentHP;

	if (CurrentHP <= KINDA_SMALL_NUMBER)
	{
		CurrentHP = 0.f;

		bIsDead = true;
		
	}

	OnUpdatedHealth.Broadcast(CurrentHP, MaxHP);
}

void UKStatusComponent::SetMaxHP(float InMaxHP)
{
	MaxHP = InMaxHP;

	if (MaxHP < KINDA_SMALL_NUMBER)
	{
		MaxHP = 0.1f;
	}

	OnUpdatedHealth.Broadcast(CurrentHP, MaxHP);
}
