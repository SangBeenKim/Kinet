#include "Gimmick/ExplosiveBarrel.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

int32 AExplosiveBarrel::bShowDebugExplosion = 0;

FAutoConsoleVariableRef CVarShowExplosionDebug(
	TEXT("Kinet.ShowExplosionDebug"),
	AExplosiveBarrel::bShowDebugExplosion,
	TEXT(""),
	ECVF_Cheat
);

AExplosiveBarrel::AExplosiveBarrel()
	: ExplosionRadius(500.f)
	, BaseDamage(50.f)
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	SetRootComponent(StaticMeshComp);

}

float AExplosiveBarrel::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageAmount > KINDA_SMALL_NUMBER)
	{
		Explode();
	}

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AExplosiveBarrel::Explode()
{
	PlayExplosionEffects();

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	UGameplayStatics::ApplyRadialDamage(
		this,
		BaseDamage,
		GetActorLocation(),
		ExplosionRadius,
		UDamageType::StaticClass(),
		IgnoreActors,
		this,
		GetInstigatorController(),
		true
	);

	// 디버그 옵션
	if (bShowDebugExplosion)
	{
		DrawDebugSphere(
			GetWorld(),
			GetActorLocation(),
			ExplosionRadius,
			32,
			FColor::Red,
			false,
			5.f,
			0,
			2.f
		);
	}

	Destroy();
}

void AExplosiveBarrel::PlayExplosionEffects()
{
	if (IsValid(ExplosionEffect))
	{
		const float ExplosionScale = ExplosionRadius / 100.f;

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(GetActorLocation());
		SpawnTransform.SetRotation(GetActorRotation().Quaternion());
		SpawnTransform.SetScale3D(FVector(ExplosionScale));

		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionEffect,
			SpawnTransform
		);
	}

	if (IsValid(ExplosionSound))
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ExplosionSound,
			GetActorLocation(),
			1.f,
			1.f,
			0.f
		);
	}

	if (IsValid(ExplosiveCameraShake))
	{
		UGameplayStatics::PlayWorldCameraShake(
			GetWorld(),
			ExplosiveCameraShake,
			GetActorLocation(),
			0.f,
			ExplosionRadius * 1.5f,
			1.0f
		);
	}
}
