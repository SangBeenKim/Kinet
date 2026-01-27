#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleLayout.generated.h"

class UMenuLayout;
class UTextBlock;
struct FMenuButtonData;

UCLASS()
class KINET_API UTitleLayout : public UUserWidget
{
	GENERATED_BODY()

public:
	UTitleLayout(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	void ShowMainMenu();
	UFUNCTION()
	void SelectMenu(const FName& InActionID);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuLayout> TitleMenu;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PressAnyKeyText;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> PressAnyKeyTextAnim;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TitleMenuData", meta = (AllowPrivateAccess))
	TArray<FMenuButtonData> TitleMenuData;
	bool bIsAnyKeyPressed;
	
};
