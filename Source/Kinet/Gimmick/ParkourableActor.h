#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ParkourableActor.generated.h"

class USplineComponent;
struct FParkourCheckResult;

UCLASS()
class KINET_API AParkourableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AParkourableActor();
	void GetLedgeTransforms(const FVector& InHitLocation, const FVector& InActorLocation, FParkourCheckResult& OutParkourResult);

protected:
	virtual void BeginPlay() override;

private:
	USplineComponent* FindLedgeClosestToActor(const FVector& InActorLocation);

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComp;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMesh;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<USplineComponent>> Ledges;
	UPROPERTY(VisibleAnywhere, Category = "Ledges")
	TMap<TObjectPtr<USplineComponent>, TObjectPtr<USplineComponent>> OppositeLedges;
	UPROPERTY(EditAnywhere, Category = "Ledges", meta = (ClampMin = "60.0", ClampMax = "100.0", Units = "cm"))
	float MinLedgeWidth;
};
