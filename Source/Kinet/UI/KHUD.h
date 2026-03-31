#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KHUD.generated.h"

class UHPBar;
class UPauseMenu;
class IHealthInterface;
class UImage;

UCLASS()
class KINET_API UKHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	void InitializeHUD(IHealthInterface* InOwner);
	void SetPauseMenuVisible(bool bVisible);
	void SetCrosshairVisible(bool bVisible);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHPBar> HUD_HPBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPauseMenu> HUD_PauseMenu;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Crosshair;

};
