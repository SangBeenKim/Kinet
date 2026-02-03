#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "KCharacterBase.generated.h"

class UAnimMontage;
class UWidgetComponent;
class UKStatusComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAttackNotifySignature, FName);

UCLASS()
class KINET_API AKCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AKCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	UKStatusComponent* GetStatusComponent() const { return StatusComp; }
	FOnAttackNotifySignature OnAttackNotify;

protected:
	virtual void BeginPlay() override;
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;
	virtual void Die();
	void InputAttackMelee();
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
