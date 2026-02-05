#pragma once

#include "CoreMinimal.h"
#include "Character/KCharacterBase.h"
#include "KPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UKInputConfig;
class UCameraShakeBase;
struct FInputActionValue;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCombatModeChanged, bool);

UCLASS()
class KINET_API AKPlayerCharacter : public AKCharacterBase
{
	GENERATED_BODY()

public:
	AKPlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual bool CanJumpInternal_Implementation() const override;

private:
	void InputMove(const FInputActionValue& InValue);
	void InputLook(const FInputActionValue& InValue);
	void InputAttack();
	void InputInteract();
	void InputAiming();
	void StopAiming();
	void SetCameraAimView(bool bIsAiming);
	void InputTest();
	virtual void Die() override;

public:
	FOnCombatModeChanged OnCombatModeChanged;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComp;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComp;
	UPROPERTY(EditDefaultsOnly, Category = "UserCustomize|Input")
	TObjectPtr<UKInputConfig> InputConfig;
	UPROPERTY(EditAnywhere, Category = "UserCustomize|InteractRange")
	float InteractRange;
	UPROPERTY(EditAnywhere, Category = "UserCustomize|InteractRange")
	float InteractRadius;
	UPROPERTY(EditDefaultsOnly, Category = "UserCustomize|Effect")
	TSubclassOf<UCameraShakeBase> AttackRangedCameraShake;
	UPROPERTY(EditAnywhere, Category = "UserCustomize|Camera")
	FVector AimCameraPos;
	UPROPERTY(EditAnywhere, Category = "UserCustomize|Camera")
	float DefaultLength;
	UPROPERTY(EditAnywhere, Category = "UserCustomize|Camera")
	float AimLength;
	UPROPERTY(EditAnywhere, Category = "UserCustomize|Camera")
	float DefaultFOV;
	UPROPERTY(EditAnywhere, Category = "UserCustomize|Camera")
	float AimFOV;

public:
	static bool bShowPlayerCharacterDebug; // 디버그

};
