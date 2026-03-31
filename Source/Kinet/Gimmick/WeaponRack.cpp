#include "Gimmick/WeaponRack.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/KPlayerCharacter.h"
#include "Items/KWeapon.h"

AWeaponRack::AWeaponRack()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	SetRootComponent(StaticMeshComp);

	DetectComp = CreateDefaultSubobject<USphereComponent>(TEXT("DetectComp"));
	DetectComp->SetupAttachment(StaticMeshComp);

	InteractWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidgetComp"));
	InteractWidgetComp->SetupAttachment(StaticMeshComp);
	InteractWidgetComp->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	InteractWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	InteractWidgetComp->SetDrawSize(FVector2D(150.f, 20.f));

}

void AWeaponRack::Interact(AActor* Interactor)
{
	if (!ensureMsgf(IsValid(WeaponClass), TEXT("WeaponClass is invalid."))) return;

	AKPlayerCharacter* PlayerCharacter = Cast<AKPlayerCharacter>(Interactor);
	if (IsValid(PlayerCharacter))
	{
		FActorSpawnParameters Params;
		Params.Owner = PlayerCharacter;
		Params.Instigator = PlayerCharacter->GetInstigator();

		if (IsValid(PlayerCharacter->GetCurrentWeapon()))
		{
			PlayerCharacter->GetCurrentWeapon()->UnequipWeapon();
			AKWeapon* SpawnedWeapon = GetWorld()->SpawnActor<AKWeapon>(WeaponClass, Params);
			if (SpawnedWeapon)
			{
				SpawnedWeapon->SetActorHiddenInGame(true);
				SpawnedWeapon->EquipWeapon(PlayerCharacter);
				SpawnedWeapon->SetActorHiddenInGame(false);
			}
			return;
		}

		AKWeapon* SpawnedWeapon = GetWorld()->SpawnActor<AKWeapon>(WeaponClass, Params);
		if (SpawnedWeapon)
		{
			SpawnedWeapon->SetActorHiddenInGame(true);
			SpawnedWeapon->EquipWeapon(PlayerCharacter);
			SpawnedWeapon->SetActorHiddenInGame(false);
		}
	}
}

void AWeaponRack::BeginPlay()
{
	Super::BeginPlay();

	InteractWidgetComp->SetVisibility(false);

	if (!DetectComp->OnComponentBeginOverlap.IsAlreadyBound(this, &ThisClass::OnOverlapBegin))
	{
		DetectComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	}

	if (!DetectComp->OnComponentEndOverlap.IsAlreadyBound(this, &ThisClass::OnOverlapEnd))
	{
		DetectComp->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnd);
	}
}

void AWeaponRack::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// UI 표시
	AKPlayerCharacter* Player = Cast<AKPlayerCharacter>(OtherActor);
	if (IsValid(Player))
	{
		InteractWidgetComp->SetVisibility(true);
	}
	
}

void AWeaponRack::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// UI 숨김
	AKPlayerCharacter* Player = Cast<AKPlayerCharacter>(OtherActor);
	if (IsValid(Player))
	{
		InteractWidgetComp->SetVisibility(false);
	}

}
