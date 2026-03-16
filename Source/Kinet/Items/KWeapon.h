#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KWeapon.generated.h"

class AKCharacterBase;

UCLASS()
class KINET_API AKWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AKWeapon();
	void ExecuteAttack();
	void EquipWeapon(AKCharacterBase* InCharacter);
	void UnequipWeapon();
	void DestroyWeapon(AKCharacterBase* InCharacter);
	void ExecuteAttackRanged();
	UAnimMontage* GetEquipWeaponMontage() { return AM_GetWeapon; }

protected:
	virtual void BeginPlay() override;
	void HandleAttackSignal(FName SignalName);
	void CreateHitTrace();
	void ResetHitHistory();

private:
	void TryFire();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimMontage")
	TObjectPtr<UAnimMontage> AM_Attack;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimMontage")
	TObjectPtr<UAnimMontage> AM_GetWeapon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimMontage")
	TObjectPtr<UAnimMontage> AM_AttackRanged;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Units = cm))
	float MaxAttackRange = 25000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimLayer")
	TSubclassOf<UAnimInstance> Anim_Weapon;

private:
	UPROPERTY()
	TArray<AActor*> HitHistory;
	FVector LastMuzzlePos;
	FVector LastGripPos;

public:
	static int32 ShowWeaponDebug; // 디버그
};
