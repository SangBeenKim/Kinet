#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "KCharacterBase.generated.h"

class UAnimMontage;

UCLASS()
class KINET_API AKCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AKCharacterBase();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMeleeMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> TakeDamageMontage;

};
