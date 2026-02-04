#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "KCharacterBase.generated.h"

class UAnimMontage;
class UKAnimInstance;
class UWidgetComponent;
class AKWeapon;
class UKStatusComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAttackNotifySignature, FName);

UCLASS()
class KINET_API AKCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AKCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	UKStatusComponent* GetStatusComponent() const { return StatusComp; }
	virtual void SetCurrentWeapon(AKWeapon* InWeapon);
	AKWeapon* GetCurrentWeapon() const { return CurrentWeapon; }
	void PlayAnimMontage(UAnimMontage* InMontage);

protected:
	virtual void OnMontageEnded(UAnimMontage* InMontage, bool bInterrupted);
	virtual void BeginPlay() override;
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;
	virtual void Die();
	void InputAttackMelee();

public:
	FOnAttackNotifySignature OnAttackNotify;

protected:
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	//TObjectPtr<UAnimMontage> AttackMeleeMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> TakeDamageMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> DeathMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI|HPBar")
	TObjectPtr<UWidgetComponent> HPBarWidgetComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Status")
	TObjectPtr<UKStatusComponent> StatusComp;
	UPROPERTY()
	TObjectPtr<UKAnimInstance> CharacterAnim;
	UPROPERTY()
	TObjectPtr<AKWeapon> CurrentWeapon;

};
