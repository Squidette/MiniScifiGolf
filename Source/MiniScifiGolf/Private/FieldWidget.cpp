// Fill out your copyright notice in the Description page of Project Settings.


#include "FieldWidget.h"
#include "Components/ProgressBar.h"

#define FIELDWIDGET_SPINBARRATIO 0.2f

UFieldWidget::UFieldWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	ShotBarValue = FIELDWIDGET_SPINBARRATIO;
}

void UFieldWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (ShotBarActivated)
	{
		UpdateShotBar(InDeltaTime);
	}
}

void UFieldWidget::PressShotBar()
{
	if (!ShotBarActivated)
	{
		ActivateShotBar();
	}
	else if (!PowerSet)
	{
		SetPower();
	}
	else if (!DirectionSet)
	{
		SetDirection();
	}
}

void UFieldWidget::ActivateShotBar()
{
	ShotBarActivated = true;
}

void UFieldWidget::SetPower()
{
	if (ShotBarValue > FIELDWIDGET_SPINBARRATIO)
	{
		PowerValue = ShotBarValue;
		PowerSet = true;
	}
}

void UFieldWidget::SetDirection()
{
	if (ShotBarValue > FIELDWIDGET_SPINBARRATIO)
	{
		DirectionValue = -10.0f;
	}
	else
	{
		DirectionValue = ShotBarValue - 0.1f;
	}

	DirectionSet = true;
}

void UFieldWidget::UpdateShotBar(const float& dt)
{
	if (ShotBarDirection) // 올라가기
	{
		ShotBarValue += ShotBarSpeed * dt;

		if (ShotBarValue > 1.0f)
		{
			ShotBarValue = 1.0f;
			ShotBarDirection = false;
		}
	}
	else // 내려가기
	{
		ShotBarValue -= ShotBarSpeed * dt;

		// 타구바 끝
		if (ShotBarValue < 0.0f)
		{
			ShotBarValue = 0.0f;

			ShotBarActivated = false;

			UE_LOG(LogTemp, Warning, TEXT("타구바: %f, %f"), PowerValue, DirectionValue);
			OnShotMade.Execute(PowerValue, DirectionValue);

			ShotBarDirection = true;
			PowerSet = false;
			DirectionSet = false;
		}
	}

	if (ShotBar)
	{
		ShotBar->SetPercent(ShotBarValue);
	}
}
