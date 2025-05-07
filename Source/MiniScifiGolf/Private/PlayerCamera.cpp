// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCamera.h"

APlayerCamera::APlayerCamera(const FObjectInitializer& ObjectInitializer)
	: ACineCameraActor(ObjectInitializer)
{
	// �÷��̾� ī�޶� �±� ���̱�
	FGameplayTag playerCameraTag = FGameplayTag::RequestGameplayTag(FName("Camera.Player"));

	if (!TagContainer.HasTag(playerCameraTag))
	{
		TagContainer.AddTag(playerCameraTag);
	}
}
