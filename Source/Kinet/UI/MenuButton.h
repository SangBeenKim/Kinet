#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/MenuButtonData.h"
#include "MenuButton.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class KINET_API UMenuButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UMenuButton(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	void InitButton(const FMenuButtonData& InData);

protected:
	UFUNCTION()
	void OnInternalButtonClicked();

public:
	FOnMenuActionSignature OnButtonClicked;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ButtonText;
	FName AssignedActionID;
};
