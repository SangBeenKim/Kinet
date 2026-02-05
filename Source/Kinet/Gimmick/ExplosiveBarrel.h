#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBarrel.generated.h"

UCLASS()
class KINET_API AExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	AExplosiveBarrel();

protected:
	virtual float TakeDamage(
		float DamageAmount, 
		struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, 
		AActor* DamageCauser
	) override;
	void Explode();
	void PlayExplosionEffects();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	UPROPERTY(EditAnywhere, Category = "UserCustomize|Effects")
	TObjectPtr<UParticleSystem> ExplosionEffect;
	UPROPERTY(EditAnywhere, Category = "UserCustomize|Effects")
	TObjectPtr<USoundBase> ExplosionSound;
	UPROPERTY(EditAnywhere, Category = "UserCustomize|Effects")
	TSubclassOf<UCameraShakeBase> ExplosiveCameraShake;
	UPROPERTY(EditAnywhere, Category = "UserCustomize")
	float ExplosionRadius;
	UPROPERTY(EditAnywhere, Category = "UserCustomize")
	float BaseDamage;
	
	
public:
	static int32 bShowDebugExplosion; // 디버그
};
