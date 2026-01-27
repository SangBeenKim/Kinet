#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "KGameInstance.generated.h"

UCLASS()
class KINET_API UKGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	void MoveToLevel(const FName& LevelName);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelData", meta = (AllowPrivateAccess))
	TMap<FName, TSoftObjectPtr<UWorld>> LevelMap;
};
