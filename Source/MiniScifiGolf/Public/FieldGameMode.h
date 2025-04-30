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
	MAP
};

UCLASS()
class MINISCIFIGOLF_API AFieldGameMode : public AGameModeBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	
	ECameraMode CurrentCameraState;

public:
	bool SetCameraMode(ECameraMode mode);
};
