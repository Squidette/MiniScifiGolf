// Fill out your copyright notice in the Description page of Project Settings.

#include "FieldGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "FieldWidget.h"
#include "Camera/CameraActor.h"
#include "PlayerCamera.h"
#include "BallCamera.h"
#include "EngineUtils.h"
#include "MapCamera.h"
#include "ResultWidget.h"
#include "../MiniScifiGolf.h"

void AFieldGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 위젯 생성
	if (FieldWidgetFactory)
	{
		FieldWidget = CreateWidget<UFieldWidget>(GetWorld(), FieldWidgetFactory);

		if (FieldWidget)
		{
			FieldWidget->AddToViewport();
		}
	}

	if (ResultWidgetFactory)
	{
		ResultWidget = CreateWidget<UResultWidget>(GetWorld(), ResultWidgetFactory);

		if (ResultWidget)
		{
			ResultWidget->UnShow();
			ResultWidget->AddToViewport();
		}
	}

	// 태그로 플레이어 카메라 찾기
	for (TActorIterator<APlayerCamera> It(GetWorld()); It; ++It)
	{
		if (It->GetTagContainer().HasTag(FGameplayTag::RequestGameplayTag(FName("Camera.Player"))))
		{
			PlayerCamera = Cast<ACameraActor>(*It);
			break;
		}
	}

	// 태그로 공 카메라 찾기
	for (TActorIterator<ABallCamera> It(GetWorld()); It; ++It)
	{
		if (It->GetTagContainer().HasTag(FGameplayTag::RequestGameplayTag(FName("Camera.Ball"))))
		{
			BallCamera = Cast<ACameraActor>(*It);
			break;
		}
	}

	// 태그로 맵 카메라 찾기
	for (TActorIterator<AMapCamera> It(GetWorld()); It; ++It)
	{
		if (It->GetTagContainer().HasTag(FGameplayTag::RequestGameplayTag(FName("Camera.Map"))))
		{
			MapCamera = *It;
			break;
		}
	}

	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	SetCameraMode(ECameraMode::PLAYER);
}

bool AFieldGameMode::CheckValidCamera(ECameraMode mode, AActor*& cameraActor)
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

	cameraActor = Cast<AActor>(cameraToSwitch);
	if (!cameraActor)
	{
		CUSTOMLOG(TEXT("%s"), TEXT("카메라를 액터로 전환실패"));
		return false;
	}

	return true;
}

bool AFieldGameMode::SetCameraMode(ECameraMode mode)
{
	AActor* cameraActorToSwitch = nullptr;

	if (CheckValidCamera(mode, cameraActorToSwitch))
	{
		if (cameraActorToSwitch)
		{
			PlayerController->SetViewTarget(cameraActorToSwitch);
			CurrentCameraState = mode;
			return true;
		}

		CUSTOMLOG(TEXT("%s"), TEXT("cameraActorToSwitch was null"));
		return false;
	}
	return false;
}

bool AFieldGameMode::SetCameraModeWithBlend(ECameraMode mode, float duration)
{
	AActor* cameraActorToSwitch = nullptr;

	if (CheckValidCamera(mode, cameraActorToSwitch))
	{
		if (cameraActorToSwitch)
		{
			PlayerController->SetViewTargetWithBlend(cameraActorToSwitch, duration, VTBlend_Cubic);
			CurrentCameraState = mode;
			return true;
		}

		CUSTOMLOG(TEXT("%s"), TEXT("cameraActorToSwitch was null"));
		return false;
	}
	return false;
}

void AFieldGameMode::MoveMapCameraVertical(float v)
{
	if (MapCamera) { MapCamera->MoveVertical(v); }
}

void AFieldGameMode::MoveMapCameraHorizontal(float v)
{
	if (MapCamera) { MapCamera->MoveHorizontal(v); }
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
		return Cast<ACameraActor>(MapCamera);
	default:
		return nullptr;
	}
}

int AFieldGameMode::GetFieldPar() const { return Par; }
int AFieldGameMode::GetCurrentShots() const { return CurrentShots; }
void AFieldGameMode::IncrementCurrentShots() { CurrentShots++; }
