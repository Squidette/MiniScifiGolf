// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class MINISCIFIGOLF_API UResultWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY()
	class AFieldGameMode* GameModeBase;

	UFUNCTION()
	void OnEndButtonClicked();
	
protected:
	virtual void NativeConstruct() override;
	
	// 현재 레벨 이름
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* MapName; // how do you set text via code

	// 현재 레벨의 파 (기준 점수)
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Par;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* YourScore;

	UPROPERTY(meta=(BindWidget))
	class UButton* EndButton;


public:
	UFUNCTION()
	void Show();

	UFUNCTION()
	void UnShow();
};
