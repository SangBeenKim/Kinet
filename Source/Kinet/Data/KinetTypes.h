#pragma once

#include "CoreMinimal.h"
#include "KinetTypes.generated.h"

UENUM(BlueprintType)
enum class EGameFlowState : uint8
{
    Playing     UMETA(DisplayName = "Playing"),
    Paused      UMETA(DisplayName = "Paused"),
    GameOver    UMETA(DisplayName = "Game Over"),
    GameClear   UMETA(DisplayName = "Game Clear"),
};
