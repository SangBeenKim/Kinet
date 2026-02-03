#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class KINET_API IInteractable
{
	GENERATED_BODY()

public:
	// 상호 작용 실행
	virtual void Interact(AActor* Interactor) = 0;
	// UI 에 표시할 이름 (ex. "검", "상자")
	virtual FText GetInteractionName() = 0;
	// UI 에 표시할 행동 이름 (ex. "줍기", "열기")
	virtual FText GetInteractionActionText() = 0;

};
