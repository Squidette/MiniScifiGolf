// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCamera.h"

APlayerCamera::APlayerCamera(const FObjectInitializer& ObjectInitializer)
	: ACineCameraActor(ObjectInitializer)
{
	// 플레이어 카메라 태그 붙이기
	FGameplayTag playerCameraTag = FGameplayTag::RequestGameplayTag(FName("Camera.Player"));

	if (!TagContainer.HasTag(playerCameraTag))
	{
		TagContainer.AddTag(playerCameraTag);
	}
}
