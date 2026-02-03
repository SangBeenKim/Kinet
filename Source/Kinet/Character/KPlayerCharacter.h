#pragma once

#include "CoreMinimal.h"
#include "Character/KCharacterBase.h"
#include "KPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UKInputConfig;
class AKWeapon;
struct FInputActionValue;

UCLASS()
class KINET_API AKPlayerCharacter : public AKCharacterBase
{
	GENERATED_BODY()

public:
	AKPlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	void SetCurrentWeapon(AKWeapon* InWeapon);

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void InputMove(const FInputActionValue& InValue);
	void InputLook(const FInputActionValue& InValue);
	void InputInteract();
	virtual void Die() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpringArm")
	TObjectPtr<USpringArmComponent> SpringArmComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UKInputConfig> InputConfig;
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<AKWeapon> CurrentWeapon;
	UPROPERTY(EditAnywhere, Category = "InteractRange")
	float InteractRange;
	UPROPERTY(EditAnywhere, Category = "InteractRange")
	float InteractRadius;
};
