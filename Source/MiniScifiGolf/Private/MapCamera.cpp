// Fill out your copyright notice in the Description page of Project Settings.


#include "MapCamera.h"

#include "Camera/CameraComponent.h"

AMapCamera::AMapCamera()
{
	// 맵 카메라 태그 붙이기
	FGameplayTag mapCameraTag = FGameplayTag::RequestGameplayTag(FName("Camera.Map"));
	if (!TagContainer.HasTag(mapCameraTag))
	{
		TagContainer.AddTag(mapCameraTag);
	}

	GetCameraComponent()->SetProjectionMode(ECameraProjectionMode::Orthographic);
	GetCameraComponent()->SetOrthoWidth(OrthoWidth);
}

void AMapCamera::MoveHorizontal(float v)
{
	FVector currentLocation = GetActorLocation();
	currentLocation.Y = currentLocation.Y + v * MoveSpeed * GetWorld()->GetDeltaSeconds();
	SetActorLocation(currentLocation);
}

void AMapCamera::MoveVertical(float v)
{
	FVector currentLocation = GetActorLocation();
	currentLocation.X = currentLocation.X + v * MoveSpeed * GetWorld()->GetDeltaSeconds();
	SetActorLocation(currentLocation);
}

void AMapCamera::BeginPlay()
{
	Super::BeginPlay();
}

void AMapCamera::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
