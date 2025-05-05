// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FieldWidget.generated.h"

/**
 *
 */
UCLASS()
class MINISCIFIGOLF_API UFieldWidget : public UUserWidget
{
	GENERATED_BODY()

	//UPROPERTY(meta = (BindWidget))
	//class UTextBlock* TextHighScore;

	// Å¸±¸¹Ù
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ShotBar;

	UPROPERTY(EditAnywhere)
	float ShotBarSpeed = 0.5f;

	UPROPERTY(EditAnywhere)
	float ShotBarValue;

	bool ShotBarActivated = true;
	bool ShotBarDirection = true; //true: up, false: down

	float PowerValue = -1.0f;
	float DirectionValue = -0.0f;

public:
	UFieldWidget(const FObjectInitializer& ObjectInitializer);

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void ActivateShotBar();

	UFUNCTION()
	void SetPower();

	UFUNCTION()
	void SetDirection();

	void UpdateShotBar(const float& dt);
};
