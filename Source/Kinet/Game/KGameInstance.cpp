#include "Game/KGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UKGameInstance::MoveToLevel(const FName& LevelName)
{
	UGameplayStatics::SetGamePaused(this, false);

	if (LevelName == "Exit")
	{
		UKismetSystemLibrary::QuitGame(
			this, 
			GetFirstLocalPlayerController(GetWorld()), 
			EQuitPreference::Quit, 
			false
		);

		return;
	}

	if (LevelMap.Contains(LevelName))
	{
		UWorld* TargetLevel = LevelMap[LevelName];
		if (IsValid(TargetLevel))
		{
			UGameplayStatics::OpenLevelBySoftObjectPtr(this, TargetLevel);
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("LevelMap[%s] is invalid."), *LevelName.ToString());
}
