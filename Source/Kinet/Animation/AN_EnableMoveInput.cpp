#include "Animation/AN_EnableMoveInput.h"
#include "Character/KCharacterBase.h"
#include "Components/KStatusComponent.h"

void UAN_EnableMoveInput::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	AKCharacterBase* Character = Cast<AKCharacterBase>(MeshComp->GetOwner());
	if (!IsValid(Character)) return;
	
	UKStatusComponent* StatusComp = Character->GetStatusComponent();
	if (!IsValid(StatusComp)) return;

	StatusComp->bIsActionLocked = false;
	StatusComp->bIsDash = false;
}
