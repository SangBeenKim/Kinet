#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "KWeapon.generated.h"

class USphereComponent;
class AKCharacterBase;

UCLASS()
class KINET_API AKWeapon : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AKWeapon();

	// ~IInteractable
	virtual void Interact(AActor* Interactor) override;
	virtual FText GetInteractionName() override { return WeaponNameText; }
	virtual FText GetInteractionActionText() override { return EquipWeaponText; }
	// ~IInteractable

	void ExecuteAttack();
	virtual UAnimMontage* Attack();
	void EquipWeapon(AKCharacterBase* InCharacter);

protected:
	virtual void BeginPlay() override;
	void HandleAttackSignal(FName SignalName);
	void CreateHitTrace();
	void ResetHitHistory();

private:
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

protected:
	UPROPERTY(EditAnywhere, Category = "UI")
	FText WeaponNameText;
	UPROPERTY(EditAnywhere, Category = "UI")
	FText EquipWeaponText;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> DetectPlayerCharacterComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimMontage")
	TObjectPtr<UAnimMontage> AM_Attack;
	//UPROPERTY(VisibleAnywhere)
	//TObjectPtr<AKPlayerCharacter> OwnerCharacter;

private:
	UPROPERTY()
	TArray<AActor*> HitHistory;
	FVector LastMuzzlePos;
	FVector LastGripPos;

};
