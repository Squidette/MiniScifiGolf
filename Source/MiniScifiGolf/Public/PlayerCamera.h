// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "PlayerCamera.generated.h"

/**
 * 
 */
UCLASS()
class MINISCIFIGOLF_API APlayerCamera : public ACameraActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

};
