// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "HoleCup.generated.h"

UCLASS()
class MINISCIFIGOLF_API AHoleCup : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHoleCup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	FGameplayTagContainer TagContainer;

public:
	const FGameplayTagContainer& GetTagContainer() { return TagContainer; }

	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxComp;
};
