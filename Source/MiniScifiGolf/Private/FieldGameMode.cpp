// Fill out your copyright notice in the Description page of Project Settings.


#include "FieldGameMode.h"

void AFieldGameMode::BeginPlay()
{
	Super::BeginPlay();

	CurrentCameraState = ECameraMode::PLAYER;
}

bool AFieldGameMode::SetCameraMode(ECameraMode mode)
{
	if (mode != CurrentCameraState) return false;
	
	CurrentCameraState = mode;

	// 여기서 카메라 바꾸기
	
	return true;
}
