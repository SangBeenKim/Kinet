#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MenuButtonData.generated.h"

USTRUCT(BlueprintType)
struct KINET_API FMenuButtonData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName ActionID;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText ButtonLabel;
	UPROPERTY(EditDefaultsOnly)
	FText Description;

	FMenuButtonData() {};
	FMenuButtonData(FName InActionID, FText InButtonLabel, FText InDescription = FText::GetEmpty())
		: ActionID(InActionID), ButtonLabel(InButtonLabel), Description(InDescription)
	{
	};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMenuActionSignature, const FName&, ActionID);