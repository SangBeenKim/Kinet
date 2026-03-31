#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KGameModeBase.generated.h"

class AKCharacterBase;
enum class EGameFlowState : uint8;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentGameStateChanged, EGameFlowState);

UCLASS()
class KINET_API AKGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AKGameModeBase();

protected:
	virtual void BeginPlay() override;

private:
	void HandleCharacterDeath(AKCharacterBase* InCharacter);

public:
	FOnCurrentGameStateChanged OnCurrentGameStateChanged;

};
