#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "KCharacterBase.generated.h"

class UAnimMontage;
class UWidgetComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMaxHPChangedDelegate, float /*InMaxHP*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentHPChangedDelegate, float /*InCurrentHP*/);
DECLARE_MULTICAST_DELEGATE(FOnOutOfCurrentHPDelegate);

UCLASS()
class KINET_API AKCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AKCharacterBase();
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;

	//Status
	float GetMaxHP() const { return MaxHP; }
	float GetCurrentHP() const { return CurrentHP; }
	void SetMaxHP(float InMaxHP);
	void SetCurrentHP(float InCurrentHP);
	bool IsDead() const { return bIsDead; }

protected:
	virtual void BeginPlay() override;
	virtual void Die();
	void InputAttackMelee();

private:
	//Test
	void TestAttack();

public:
	FOnMaxHPChangedDelegate OnMaxHPChanged;
	FOnCurrentHPChangedDelegate OnCurrentHPChanged;
	FOnOutOfCurrentHPDelegate OnOutOfCurrentHP;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMeleeMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> TakeDamageMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> DeathMontage;

	//Status
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float CurrentHP = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float MaxHP = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	uint8 bIsDead : 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI|HPBar")
	TObjectPtr<UWidgetComponent> HPBarWidgetComp;

};
