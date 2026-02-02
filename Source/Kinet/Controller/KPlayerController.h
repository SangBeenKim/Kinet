#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UPauseMenu;
class UKHUD;

UCLASS()
class KINET_API AKPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	void TogglePauseMenu();
	UFUNCTION()
	void HandleMenuAction(const FName& InActionID);
	void CreateHUD();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess))
	TObjectPtr<UInputMappingContext> IMC_Default;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> IA_Pause;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI|HUD", meta = (AllowPrivateAccess))
	TSubclassOf<UKHUD> HUDClass;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UKHUD> HUDInstance;

};
