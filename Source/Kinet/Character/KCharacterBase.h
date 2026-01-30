#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "KCharacterBase.generated.h"

class UAnimMontage;
class UWidgetComponent;
class UKStatusComponent;

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

protected:
	virtual void BeginPlay() override;
	virtual void Die();
	void InputAttackMelee();

private:
	//Test
	void TestAttack();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMeleeMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> TakeDamageMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> DeathMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI|HPBar")
	TObjectPtr<UWidgetComponent> HPBarWidgetComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Status")
	TObjectPtr<UKStatusComponent> StatusComp;
};
