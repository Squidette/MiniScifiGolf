// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "BallGuide.generated.h"

// 공의 위치를 매 프레임 업데이트 받음
// 카메라 도우미

UCLASS()
class MINISCIFIGOLF_API ABallGuide : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	class AGolfBallBase* Ball = nullptr;

	UPROPERTY()
	FGameplayTagContainer TagContainer;

public:
	// Sets default values for this actor's properties
	ABallGuide();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* SpringArmComp;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	const FGameplayTagContainer& GetTagContainer() { return TagContainer; }

	USpringArmComponent* GetSpringArmComp() const { return SpringArmComp; }
};
