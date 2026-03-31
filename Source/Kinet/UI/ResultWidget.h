#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/MenuButtonData.h"
#include "ResultWidget.generated.h"

class UTextBlock;
class UMenuLayout;

UCLASS()
class KINET_API UResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void CreateResult(const FText& InResultText,const TArray<FMenuButtonData>& MenuDataList);

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void SelectMenu(const FName& InActionID);

public:
	FOnMenuActionSignature OnMenuActionSignature;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuLayout> ResultMenu;
	
};
