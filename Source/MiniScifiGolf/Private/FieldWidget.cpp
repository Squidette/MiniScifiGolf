// Fill out your copyright notice in the Description page of Project Settings.


#include "FieldWidget.h"
#include "Components/ProgressBar.h"
#include "../MiniScifiGolf.h"

// 타구바 중에서 왼쪽의 정확도 부분이 될 비율
#define FIELDWIDGET_SPINBARRATIO 0.2f

UFieldWidget::UFieldWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	ShotBarValue = FIELDWIDGET_SPINBARRATIO;
}

void UFieldWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (CurrentState != EShotBarState::INACTIVE)
	{
		UpdateShotBar(InDeltaTime);
	}
}

bool UFieldWidget::PressShotBar()
{
	switch (CurrentState)
	{
	case EShotBarState::INACTIVE:
		CUSTOMLOG(TEXT("%s"), TEXT("타구바 활성화됨"));
		ResetShotBarPrivateProperties();
		CurrentState = EShotBarState::WAITINGFORPOWERINPUT;
		return true;
	case EShotBarState::WAITINGFORPOWERINPUT:
		SetPower();
		CurrentState = EShotBarState::WAITINGFORPRECISIONINPUT;
		return false;
	case EShotBarState::WAITINGFORPRECISIONINPUT:
		SetPrecision();
		CurrentState = EShotBarState::WAITINGFOREND;
		return false;
	default:
		CUSTOMLOG(TEXT("%s"), TEXT("타구바는 더이상의 인풋을 받지않는다"));
		return false;
	}
}

void UFieldWidget::SetShotFail(bool b)
{
	ShotFail = b;
}

void UFieldWidget::ResetShotBarPrivateProperties()
{
	PowerValue = -1.0f;
	PrecisionValue = -1.0f;
	ShotFail = false;
	ShotBarDirection = true;
	ShotBarValue = FIELDWIDGET_SPINBARRATIO;
}

void UFieldWidget::SetPower()
{
	if (ShotBarValue > FIELDWIDGET_SPINBARRATIO)
	{
		PowerValue = ShotBarValue;
	}
	else
	{
		SetShotFail(true);
	}
}

void UFieldWidget::SetPrecision()
{
	if (ShotBarValue < FIELDWIDGET_SPINBARRATIO)
	{
		//PrecisionValue = ShotBarValue - 0.1f;

		// [0 ~ (타구바비율)] 사이의 값을 [-1.0f ~ 1.0f] 사이의 값으로 매핑
		PrecisionValue = FMath::GetMappedRangeValueClamped(
			FVector2D(0.0f, FIELDWIDGET_SPINBARRATIO),
			FVector2D(-1.0f, 1.0f),
			ShotBarValue
			);
	}
	else
	{
		SetShotFail(true);
	}
}

void UFieldWidget::UpdateShotBar(const float& dt)
{
	// 타구바가 올라감
	if (ShotBarDirection)
	{
		ShotBarValue += ShotBarSpeed * dt;

		if (ShotBarValue > 1.0f)
		{
			ShotBarValue = 1.0f;
			ShotBarDirection = false;
		}
	}
	// 타구바가 내려감
	else
	{
		ShotBarValue -= ShotBarSpeed * dt;

		// 밸류가 0으로 되돌아오면 샷을 끝냄
		if (ShotBarValue < 0.0f)
		{
			ShotBarValue = 0.0f;

			CurrentState = EShotBarState::INACTIVE;

			if (ShotFail) { CUSTOMLOG(TEXT("샷 실패")); }
			else { CUSTOMLOG(TEXT("타구바 비활성화됨, %f의 힘,  %f의 정확도"), PowerValue, PrecisionValue); }
			
			// 샷에서 정해진 정보를 발송
			OnShotBarEnded.Execute(!ShotFail, PowerValue, PrecisionValue);
		}
	}

	if (ShotBar)
	{
		ShotBar->SetPercent(ShotBarValue);
	}
}

EShotBarState UFieldWidget::GetState()
{
	return CurrentState;
}
