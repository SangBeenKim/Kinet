#include "Game/KGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UKGameInstance::MoveToLevel(const FName& LevelName)
{
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
		TSoftObjectPtr<UWorld> LevelPtr = LevelMap[LevelName];
		if (LevelPtr.IsPending())
		{
			LevelPtr.LoadSynchronous();
		}

		if (LevelPtr.IsValid())
		{
			UGameplayStatics::OpenLevelBySoftObjectPtr(this, LevelPtr);
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("LevelName is invalid."));
}
