// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "GameplayTagContainer.h"
#include "MapCamera.generated.h"

/**
 * 
 */
UCLASS()
class MINISCIFIGOLF_API AMapCamera : public ACameraActor
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTagContainer TagContainer;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	float OrthoWidth = 21000.0f;

	UPROPERTY(EditAnywhere)
	float MoveSpeed = 8000.0f;
	
public:
	AMapCamera();
	
	void MoveHorizontal(float v);
	void MoveVertical(float v);
	
	const FGameplayTagContainer& GetTagContainer() { return TagContainer; }
};
