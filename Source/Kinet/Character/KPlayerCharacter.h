#pragma once

#include "CoreMinimal.h"
#include "Character/KCharacterBase.h"
#include "KPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UKInputConfig;
struct FInputActionValue;

UCLASS()
class KINET_API AKPlayerCharacter : public AKCharacterBase
{
	GENERATED_BODY()

public:
	AKPlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void InputMove(const FInputActionValue& InValue);
	void InputLook(const FInputActionValue& InValue);
	virtual void Die() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpringArm")
	TObjectPtr<USpringArmComponent> SpringArmComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UKInputConfig> InputConfig;

};
