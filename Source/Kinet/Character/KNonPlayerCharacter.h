#pragma once

#include "CoreMinimal.h"
#include "Character/KCharacterBase.h"
#include "KNonPlayerCharacter.generated.h"

DECLARE_DELEGATE_TwoParams(FOnAttackMontageEnded, UAnimMontage*, bool /*bInterrupted*/)

UCLASS()
class KINET_API AKNonPlayerCharacter : public AKCharacterBase
{
	GENERATED_BODY()
	
public:
	AKNonPlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void BeginAttack();

protected:
	virtual void OnMontageEnded(UAnimMontage* InMontage, bool bInterrupted) override;
	virtual void Die() override;

public:
	bool bIsNowAttacking;
	FOnAttackMontageEnded OnAttackMontageEndedDelegate;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Weapon")
	TSubclassOf<AKWeapon> DefaultWeaponClass;

};
