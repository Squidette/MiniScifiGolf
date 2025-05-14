// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FieldWidget.generated.h"

/**
 * 타구바
 */
UENUM(BlueprintType)
enum class EShotBarState : uint8
{
	INACTIVE,
	WAITINGFORPOWERINPUT,
	WAITINGFORPRECISIONINPUT,
	WAITINGFOREND
	 // 끝
};

//DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnShotSignature, float, power, float, dir);
DECLARE_DELEGATE_ThreeParams(FOnShotBarEnded, bool, float, float);

UCLASS()
class MINISCIFIGOLF_API UFieldWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ShotBar;

	UPROPERTY(EditAnywhere)
	float ShotBarSpeed = 0.5f;

	//UPROPERTY(EditAnywhere)
	float ShotBarValue;

	// 타구바의 상태 변수
	EShotBarState CurrentState;

	bool ShotBarDirection = true; //true: up, false: down

	/// 타구바에서 정해지는 값 3개
	
	// 0.0f~1.0f (정상)
	float PowerValue = -1.0f; 

	// -0.1f~0.1f (정상)
	float PrecisionValue = -1.0f;

	// 샷의 실패 여부 기록
	bool ShotFail = false;
	void SetShotFail(bool b);

	void ResetShotBarPrivateProperties();

public:
	UFieldWidget(const FObjectInitializer& ObjectInitializer);

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	FOnShotBarEnded OnShotBarEnded;

	// 타구바가 활성화되는 시점에만 true 반환
	UFUNCTION()
	bool PressShotBar();

	UFUNCTION()
	void SetPower();

	UFUNCTION()
	void SetPrecision();

	void UpdateShotBar(const float& dt);
	
	EShotBarState GetState();
	
	bool GetShotFail() const { return ShotFail; }
};
