#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenu.generated.h"

class UMenuLayout;
class UTextBlock;
struct FMenuButtonData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedMenu, const FName&, InValue);

UCLASS()
class KINET_API UPauseMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void SelectMenu(const FName& InActionID);

public:
	FOnSelectedMenu OnSelectedMenu;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuLayout> PauseMenu;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PauseText;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PauseMenuData", meta = (AllowPrivateAccess))
	TArray<FMenuButtonData> PauseMenuData;

};
