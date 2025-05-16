// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FieldGameMode.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ECameraMode : uint8
{
	PLAYER,
	BALL,
	MAP,
	NONE // 끝
};

UCLASS()
class MINISCIFIGOLF_API AFieldGameMode : public AGameModeBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	// UI
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UFieldWidget> FieldWidgetFactory;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UResultWidget> ResultWidgetFactory;

	// 카메라
	ECameraMode CurrentCameraState = ECameraMode::NONE;

	UPROPERTY(EditAnywhere)
	class ACameraActor* PlayerCamera;

	UPROPERTY(EditAnywhere)
	class ACameraActor* BallCamera;

	UPROPERTY(EditAnywhere)
	class AMapCamera* MapCamera;

	bool CheckValidCamera(ECameraMode mode, AActor*& cameraActor);
	
	// 점수
	int Par = 4;			// 현재 맵의 기존 파
	int CurrentShots = 0;	// 현재 탸수

	// 현재 맵의 이름
	FString MapName = TEXT("연습용 필드");

public:
	bool SetCameraMode(ECameraMode mode);
	bool SetCameraModeWithBlend(ECameraMode mode, float duration = 1.0f);

	UPROPERTY()
	class UFieldWidget* FieldWidget;

	UPROPERTY()
	class UResultWidget* ResultWidget;

	void MoveMapCameraVertical(float v);
	void MoveMapCameraHorizontal(float v);

	const ACameraActor* GetPlayerCamera() const { return PlayerCamera; }

	int GetFieldPar() const;
	int GetCurrentShots() const;
	void IncrementCurrentShots();

private:
	UPROPERTY(EditAnywhere)
	APlayerController* PlayerController;

	ACameraActor* GetCameraActorByMode(ECameraMode mode) const;
};
