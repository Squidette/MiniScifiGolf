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

void UFieldWidget::ActivateShotBar()
{
	if (!ShotBarActivated)
	{
		ShotBarActivated = true;
	}
}

void UFieldWidget::SetPower()
{
}

void UFieldWidget::SetDirection()
{
}

void UFieldWidget::UpdateShotBar(const float& dt)
{
	if (ShotBarDirection) // up
	{
		ShotBarValue += ShotBarSpeed * dt;

		if (ShotBarValue > 1.0f)
		{
			ShotBarValue = 1.0f;
			ShotBarDirection = false;
		}
	}
	else // down
	{
		ShotBarValue -= ShotBarSpeed * dt;

		if (ShotBarValue < 0.0f)
		{
			ShotBarValue = 0.0f;
			ShotBarDirection = true;
		}
	}

	if (ShotBar)
	{
		ShotBar->SetPercent(ShotBarValue);
	}
}
