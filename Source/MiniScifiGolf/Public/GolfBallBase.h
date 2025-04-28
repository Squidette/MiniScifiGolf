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

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCameraComponent* CameraComp;
	
	// 공 세팅시 정해지는 수치 (타구바에 따라 최종수치는 달라질수 있음)
	UPROPERTY(EditAnywhere)
	FVector ImpulseAmount; // 세기

	UPROPERTY(EditAnywhere)
	FVector TorqueAmount; // 
	
	// 마그누스 효과
	UPROPERTY(EditAnywhere)
	float MagnusScalar = 0.01f;

	UPROPERTY(EditAnywhere)
	float LinearDamping_InAir = 0.05f;

	UPROPERTY(EditAnywhere)
	float LinearDamping_AfterGroundHit = 0.85f;

	UPROPERTY(EditAnywhere)
	float AngularDamping_InAir = 0.05f;

	UPROPERTY(EditAnywhere)
	float AnglularDamping_AfterGroundHit = 0.05f;
	
	// 공 치기!
	UFUNCTION()
	void Shot();

	bool HitGround = false;

	// VISUALIZATION
	UPROPERTY(EditAnywhere)
	float VisualizationCycle = 0.02f;
	FTimerHandle VisualizationHandle;
	FVector LastLocation;
	void Visualize();
	
	// 시뮬레이션용
	bool simulated = false;

	bool fasterSimulation = false;
	
	FTransform InitialTransform;
	
	UFUNCTION()
	void ResetGolfBall();

	UFUNCTION()
	void OnFirstCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
