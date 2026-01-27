#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KPlayerController_Title.generated.h"

class UTitleLayout;

UCLASS()
class KINET_API AKPlayerController_Title : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UTitleLayout> TitleWidget;
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess))
	TSubclassOf<UTitleLayout> TitleWidgetClass;

};
