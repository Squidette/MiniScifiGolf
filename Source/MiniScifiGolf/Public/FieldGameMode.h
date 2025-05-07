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
	TSubclassOf<class UUserWidget> FieldWidgetFactory;

	// 카메라
	ECameraMode CurrentCameraState = ECameraMode::NONE;

	UPROPERTY(EditAnywhere)
	class ACameraActor* PlayerCamera;

	UPROPERTY(EditAnywhere)
	class ACameraActor* BallCamera;

	UPROPERTY(EditAnywhere)
	class AMapCamera* MapCamera;

	bool CheckValidCamera(ECameraMode mode, AActor*& cameraActor);
public:
	bool SetCameraMode(ECameraMode mode);
	bool SetCameraModeWithBlend(ECameraMode mode, float duration = 1.0f);
	
	UPROPERTY()
	class UFieldWidget* FieldWidget;

	void MoveMapCameraVertical(float v);
	void MoveMapCameraHorizontal(float v);

private:
	UPROPERTY(EditAnywhere)
	APlayerController* PlayerController;

	ACameraActor* GetCameraActorByMode(ECameraMode mode) const;
};
