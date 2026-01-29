#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HPBar.generated.h"

class AKCharacterBase;
class UProgressBar;
class UTextBlock;

UCLASS()
class KINET_API UHPBar : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeHPBarWidget(AKCharacterBase* InCharacter);
	void UpdateProgressBar();
	UFUNCTION()
	void OnMaxHPChange(float InMaxHP);
	UFUNCTION()
	void OnCurrentHPChange(float InCurrentHP);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPProgressBar;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentHPText;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxHPText;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> OwningActor;
	float LastUpdatedMaxHP = 0.f;
	float LastUpdatedCurrentHP = 0.f;

};
