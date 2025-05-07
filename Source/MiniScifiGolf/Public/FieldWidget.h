// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FieldWidget.generated.h"

/**
 *
 */
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnShotSignature, float, power, float, dir);

UCLASS()
class MINISCIFIGOLF_API UFieldWidget : public UUserWidget
{
	GENERATED_BODY()

	//UPROPERTY(meta = (BindWidget))
	//class UTextBlock* TextHighScore;

	// 타구바
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ShotBar;

	UPROPERTY(EditAnywhere)
	float ShotBarSpeed = 0.5f;

	UPROPERTY(EditAnywhere)
	float ShotBarValue;

	// 차례로 true가 된다
	bool ShotBarActivated = false;
	bool PowerSet = false;
	bool DirectionSet = false;

	bool ShotBarDirection = true; //true: up, false: down

	// 타구바에서 정해지는 값
	
	// 0.0f~1.0f (정상값), -1.0f (없음), 
	float PowerValue = -1.0f; 

	// -1.0f~1.0f (정상값), -1.0f (없음), < -5.0f (실패)
	float DirectionValue = -1.0f;

public:
	UFieldWidget(const FObjectInitializer& ObjectInitializer);

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// 이벤트
	UPROPERTY()
	FOnShotSignature OnShotMade;

	UFUNCTION()
	void PressShotBar();

	// 샷에 필요한 세 가지 인풋
	UFUNCTION()
	void ActivateShotBar();

	UFUNCTION()
	void SetPower();

	UFUNCTION()
	void SetDirection();

	void UpdateShotBar(const float& dt);
};
