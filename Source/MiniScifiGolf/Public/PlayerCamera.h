// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "GameplayTagContainer.h"
#include "PlayerCamera.generated.h"

/**
 * 
 */
UCLASS()
class MINISCIFIGOLF_API APlayerCamera : public ACameraActor
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTagContainer TagContainer;
	
public:
	APlayerCamera();

	const FGameplayTagContainer& GetTagContainer() { return TagContainer; }
};
