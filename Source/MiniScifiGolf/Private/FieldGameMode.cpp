// Fill out your copyright notice in the Description page of Project Settings.

#include "FieldGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "PlayerCamera.h"
#include "EngineUtils.h"
#include "../MiniScifiGolf.h"

void AFieldGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	for (TActorIterator<APlayerCamera> It(GetWorld()); It; ++It)
	{
		if (It->GetTagContainer().HasTag(FGameplayTag::RequestGameplayTag(FName("Camera.Player"))))
		{
			PlayerCamera = Cast<ACameraActor>(*It);
			break;
		}
	}
	
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	SetCameraMode(ECameraMode::PLAYER);
}

bool AFieldGameMode::SetCameraMode(ECameraMode mode)
{
	if (mode == CurrentCameraState)
	{
		CUSTOMLOG(TEXT("%s"), TEXT("이미 설정된 카메라 모드임"));
		return false;
	}
	if (!PlayerController)
	{
		CUSTOMLOG(TEXT("%s"), TEXT("플레이어 컨트롤러를 찾을 수 없음"));
		return false;
	}

	ACameraActor* cameraToSwitch = GetCameraActorByMode(mode);
	if (!cameraToSwitch)
	{
		CUSTOMLOG(TEXT("%s"), TEXT("카메라 null"));
		return false;
	}

	AActor* cameraActorToSwitch = Cast<AActor>(cameraToSwitch);
	if (!cameraActorToSwitch)
	{
		CUSTOMLOG(TEXT("%s"), TEXT("카메라를 액터로 전환실패"));
		return false;
	}

	bool blend = true;

	if (blend)
	{
		PlayerController->SetViewTargetWithBlend(cameraActorToSwitch, 1.0f, VTBlend_Cubic);
	}
	else
	{
		PlayerController->SetViewTarget(cameraActorToSwitch);
	}

	CurrentCameraState = mode;

	return true;
}

ACameraActor* AFieldGameMode::GetCameraActorByMode(ECameraMode mode) const
{
	switch (mode)
	{
	case ECameraMode::PLAYER:
		return PlayerCamera;
	case ECameraMode::BALL:
		return BallCamera;
	case ECameraMode::MAP:
		return MapCamera;
	default:
		return nullptr;
	}
}
