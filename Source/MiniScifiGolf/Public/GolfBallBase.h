// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GolfBallBase.generated.h"

UCLASS()
class MINISCIFIGOLF_API AGolfBallBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGolfBallBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereComp;
	
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMeshComp;
	
	// 공 세팅시 정해지는 수치 (타구바에 따라 최종수치는 달라질수 있음)
	UPROPERTY(EditAnywhere)
	FVector ImpulseAmount; // 세기

	UPROPERTY(EditAnywhere)
	FVector TorqueAmount; // 
	
	// MAGNUS EFFECT
	UPROPERTY(EditAnywhere)
	float MagnusScalar = 0.01f;

	// 공 치기!
	UFUNCTION()
	void Shot();

	// VISUALIZATION
	UPROPERTY(EditAnywhere)
	float VisualizationCycle = 0.02f;
	FTimerHandle VisualizationHandle;
	FVector LastLocation;
	void Visualize();
	
	// 시뮬레이션용
	bool simulated = false;
	
	FTransform InitialTransform;
	
	UFUNCTION()
	void ResetGolfBall();
};
