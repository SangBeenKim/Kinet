#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HPBar.generated.h"

class IHealthInterface;
class UProgressBar;
class UTextBlock;

UCLASS()
class KINET_API UHPBar : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeHPBarWidget(IHealthInterface* InOwner);
	UFUNCTION()
	void UpdateProgressBar(float InCurrentHP, float InMaxHP);
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPProgressBar;

private:
	// 델리게이트 제거용 포인터
	IHealthInterface* OwnerInterface;
	TWeakObjectPtr<UObject> Owner;
};
