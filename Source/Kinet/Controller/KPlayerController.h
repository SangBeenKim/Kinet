#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UPauseMenu;

UCLASS()
class KINET_API AKPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void TogglePauseMenu();
	void CheckPauseMenuWidget();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess))
	TObjectPtr<UInputMappingContext> IMC_Default;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> IA_Pause;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPauseMenu> PauseMenuInstance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Pause", meta = (AllowPrivateAccess))
	TSubclassOf<UPauseMenu> PauseMenuClass;

};
