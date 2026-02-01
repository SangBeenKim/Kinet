#pragma once

#include "CoreMinimal.h"
#include "Character/KCharacterBase.h"
#include "KNonPlayerCharacter.generated.h"

UCLASS()
class KINET_API AKNonPlayerCharacter : public AKCharacterBase
{
	GENERATED_BODY()
	
public:
	AKNonPlayerCharacter();
	virtual void BeginPlay() override;

protected:
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;

};
