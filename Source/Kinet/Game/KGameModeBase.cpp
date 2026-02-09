#include "Game/KGameModeBase.h"
#include "EngineUtils.h"
#include "Character/KPlayerCharacter.h"
#include "Data/KinetTypes.h"

AKGameModeBase::AKGameModeBase()
{
}

void AKGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<AKCharacterBase> It(GetWorld()); It; ++It)
	{
		AKCharacterBase* Character = *It;
		if (Character)
		{
			Character->OnCharacterDead.RemoveAll(this);
			Character->OnCharacterDead.AddUObject(this, &ThisClass::HandleCharacterDeath);
		}
	}

}

void AKGameModeBase::HandleCharacterDeath(AKCharacterBase* InCharacter)
{
	if(!IsValid(InCharacter)) return;

	if (AKPlayerCharacter* Player = Cast<AKPlayerCharacter>(InCharacter))
	{
		OnCurrentGameStateChanged.Broadcast(EGameFlowState::GameOver);
	}
	else
	{
		OnCurrentGameStateChanged.Broadcast(EGameFlowState::GameClear);
	}

}
