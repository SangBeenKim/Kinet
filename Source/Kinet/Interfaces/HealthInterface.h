#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HealthInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdatedHealth, float, CurrentHealth, float, MaxHealth);

UINTERFACE(MinimalAPI)
class UHealthInterface : public UInterface
{
	GENERATED_BODY()
};

class KINET_API IHealthInterface
{
	GENERATED_BODY()

public:
	// 체력 변경시 UI 업데이트를 위한 델리게이트 반환
	virtual FOnUpdatedHealth& GetOnUpdatedHealthDelegate() = 0;
	
	// 델리게이트 바인딩후 값 초기화용으로 사용
	virtual float GetCurrentHealth() const = 0;
	virtual float GetMaxHealth() const = 0;

};
