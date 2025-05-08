// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FieldWidget.generated.h"

/**
 * 타구바
 */
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnShotSignature, float, power, float, dir);

UCLASS()
class MINISCIFIGOLF_API UFieldWidget : public UUserWidget
{
	GENERATED_BODY()
	
	// Ÿ����
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ShotBar;

	UPROPERTY(EditAnywhere)
	float ShotBarSpeed = 0.5f;

	UPROPERTY(EditAnywhere)
	float ShotBarValue;

	// ���ʷ� true�� �ȴ�
	bool ShotBarActivated = false;
	bool PowerSet = false;
	bool DirectionSet = false;

	bool ShotBarDirection = true; //true: up, false: down

	// 타구바에서 정해지는 값 2개
	
	// 0.0f~1.0f (정상), -1.0f (없음), 
	float PowerValue = -1.0f; 

	// -1.0f~1.0f (정상), -1.0f (없음), < -5.0f (FAIL)
	float DirectionValue = -1.0f;

public:
	UFieldWidget(const FObjectInitializer& ObjectInitializer);

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// �̺�Ʈ
	UPROPERTY()
	FOnShotSignature OnShotMade;

	UFUNCTION()
	void PressShotBar();

	// ���� �ʿ��� �� ���� ��ǲ
	UFUNCTION()
	void ActivateShotBar();

	UFUNCTION()
	void SetPower();

	UFUNCTION()
	void SetDirection();

	void UpdateShotBar(const float& dt);

	inline bool GetShotBarActivated() const { return ShotBarActivated; }
};
