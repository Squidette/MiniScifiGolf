// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CineCameraActor.h"
#include "GameplayTagContainer.h"
#include "BallCamera.generated.h"

/**
 * 
 */

UCLASS()
class MINISCIFIGOLF_API ABallCamera : public ACineCameraActor
{
	GENERATED_BODY()
	
	UPROPERTY()
	FGameplayTagContainer TagContainer;

protected:
	void BeginPlay() override;

public:
	ABallCamera(const FObjectInitializer& ObjectInitializer);

	const FGameplayTagContainer& GetTagContainer() { return TagContainer; }
};
