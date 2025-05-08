// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CineCameraActor.h"
#include "GameplayTagContainer.h"
#include "PlayerCamera.generated.h"

/**
 * 
 */

UCLASS()
class MINISCIFIGOLF_API APlayerCamera : public ACineCameraActor
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTagContainer TagContainer;
	
public:
	APlayerCamera(const FObjectInitializer& ObjectInitializer);

	const FGameplayTagContainer& GetTagContainer() { return TagContainer; }

	void SetLocation(const FVector& NewLocation);
};
