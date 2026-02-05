#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "WeaponRack.generated.h"

class USphereComponent;
class UWidgetComponent;
class AKWeapon;

UCLASS()
class KINET_API AWeaponRack : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AWeaponRack();
	// ~IInteractable
	virtual void Interact(AActor* Interactor) override;
	// ~IInteractable

protected:
	virtual void BeginPlay() override;

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
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> DetectComp;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> InteractWidgetComp;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AKWeapon> WeaponClass;

};
