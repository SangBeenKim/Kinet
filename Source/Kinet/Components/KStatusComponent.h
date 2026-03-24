#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/HealthInterface.h"
#include "KStatusComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KINET_API UKStatusComponent : public UActorComponent, public IHealthInterface
{
	GENERATED_BODY()

public:	
	UKStatusComponent();
	float ApplyDamage(float InDamage);

	// ~ IHealthInterface
	virtual FOnUpdatedHealth& GetOnUpdatedHealthDelegate() override { return OnUpdatedHealth; }
	virtual float GetCurrentHealth() const override { return CurrentHP; }
	virtual float GetMaxHealth() const override { return MaxHP; }
	// ~ IHealthInterface

	void SetCurrentHP(float InCurrentHP);
	void SetMaxHP(float InMaxHP);
	bool IsDead() const { return bIsDead; }
	
public:
	UPROPERTY(Transient)
	uint8 bIsActionLocked : 1;
	UPROPERTY(Transient)
	uint8 bIsAiming : 1;
	UPROPERTY(Transient)
	uint8 bIsDash : 1;

private:
	FOnUpdatedHealth OnUpdatedHealth;
	UPROPERTY(Transient)
	float CurrentHP;
	UPROPERTY()
	float MaxHP;
	UPROPERTY(Transient)
	uint8 bIsDead : 1;

};
