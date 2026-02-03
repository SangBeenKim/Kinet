#include "Items/KWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Character/KCharacterBase.h"

AKWeapon::AKWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	SetRootComponent(StaticMeshComp);

	DetectPlayerCharacterComp = CreateDefaultSubobject<USphereComponent>(TEXT("DetectPlayerCharacterComp"));
	DetectPlayerCharacterComp->SetupAttachment(StaticMeshComp);

}

void AKWeapon::Interact(AActor* Interactor)
{
	if (Owner != nullptr) return;

	if (AKCharacterBase* PlayerCharacter = Cast<AKCharacterBase>(Interactor))
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		AttachToComponent(PlayerCharacter->GetMesh(), AttachmentRules, FName(TEXT("hand_rSocket")));
		SetOwner(PlayerCharacter);
		SetActorEnableCollision(false);
		StaticMeshComp->SetSimulatePhysics(false);
	}

}

UAnimMontage* AKWeapon::Attack()
{
	return nullptr;
}

void AKWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (!ensureMsgf(IsValid(DetectPlayerCharacterComp), TEXT("SphereComponent is invalid.")))
	{
		return;
	}

	if (!DetectPlayerCharacterComp->OnComponentBeginOverlap.IsAlreadyBound(this, &ThisClass::OnOverlapBegin))
	{
		DetectPlayerCharacterComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	}

	if (!DetectPlayerCharacterComp->OnComponentEndOverlap.IsAlreadyBound(this, &ThisClass::OnOverlapEnd))
	{
		DetectPlayerCharacterComp->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnd);
	}

}

void AKWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// UI 위젯 표시
}

void AKWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// UI 위젯 숨김
}

