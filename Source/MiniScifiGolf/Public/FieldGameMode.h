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
	
	ECameraMode CurrentCameraState = ECameraMode::NONE;


	// these are game mode base properties. how do you get these cameras, when instance is not in level editor but gamemodebase is instantiated upon playing?
	UPROPERTY(EditAnywhere)
	class ACameraActor* PlayerCamera;

	UPROPERTY(EditAnywhere)
	class ACameraActor* BallCamera;

	UPROPERTY(EditAnywhere)
	class ACameraActor* MapCamera;

public:
	bool SetCameraMode(ECameraMode mode);

private:
	UPROPERTY(EditAnywhere)
	APlayerController* PlayerController;

	ACameraActor* GetCameraActorByMode(ECameraMode mode) const;
};
