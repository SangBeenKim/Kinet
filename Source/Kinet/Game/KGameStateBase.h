#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Data/KinetTypes.h"
#include "KGameStateBase.generated.h"

class UResultWidget;
class AKPlayerController;

UCLASS()
class KINET_API AKGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	EGameFlowState GetCurrentGameState() { return CurrentGameState; }

private:
	void CreateResultWidget(EGameFlowState InCurrentGameState);
	void CreateResultMenu(AKPlayerController* InPlayerController);

private:
	EGameFlowState CurrentGameState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UserCustomize|UI", meta = (AllowPrivateAccess))
	TSubclassOf<UResultWidget> ResultWidgetClass;
	UPROPERTY()
	TObjectPtr<UResultWidget> ResultWidget;
};
