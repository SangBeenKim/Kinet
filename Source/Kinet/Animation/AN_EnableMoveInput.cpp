#include "Animation/AN_EnableMoveInput.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"

void UAN_EnableMoveInput::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
	if (!Character)
	{
		return;
	}

	AController* Controller = Character->GetController();
	if (!Controller)
	{
		return;
	}

	Controller->SetIgnoreMoveInput(false);
}
