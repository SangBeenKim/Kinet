#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/MenuButtonData.h"
#include "MenuLayout.generated.h"

class UVerticalBox;

UCLASS()
class KINET_API UMenuLayout : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BuildMenu(const TArray<FMenuButtonData>& MenuDataList);

protected:
	UFUNCTION()
	void HandleButtonAction(const FName& InActionID);

public:
	UPROPERTY(BlueprintAssignable)
	FOnMenuActionSignature OnMenuActionTriggerd;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI|Layout")
	FMargin ButtonPadding;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> ButtonList;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> MenuButtonClass;

};
