#include "Animation/ANS_MeleeAttackCheck.h"
#include "Character/KCharacterBase.h"

void UANS_MeleeAttackCheck::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
	{
		if (AKCharacterBase* Character = Cast<AKCharacterBase>(AnimInstance->GetOwningActor()))
		{
			Character->OnAttackNotify.Broadcast(TEXT("AttackBegin"));
		}
	}
}

void UANS_MeleeAttackCheck::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
	{
		if (AKCharacterBase* Character = Cast<AKCharacterBase>(AnimInstance->GetOwningActor()))
		{
			Character->OnAttackNotify.Broadcast(TEXT("MeleeAttack"));
		}
	}
}
