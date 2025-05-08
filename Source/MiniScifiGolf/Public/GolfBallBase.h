// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GolfBallBase.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnBallStopped);

UCLASS()
class MINISCIFIGOLF_API AGolfBallBase : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTagContainer TagContainer;

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
	// 기본 컴포넌트
	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereComp;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditAnywhere)
	class UArrowComponent* ArrowComp;

	bool PhysicsSimulated = false;
	bool PhysicsSimulate(bool v);

	// 공 세팅시 정해지는 수치 (타구바에 따라 최종수치는 달라질수 있음)
	UPROPERTY(EditAnywhere) // 발사각, 0~90사이
	float LaunchAngleDegree = 40.0f;

	UPROPERTY(EditAnywhere)
	float LaunchFullForce = 950.0f; // 100% 힘의 세기로, 채 종류에 의해 결정

	UPROPERTY(EditAnywhere)
	float AccuracyRate = 5.0f; // 샷의 정확도 (양수, 0에 가까울수록 정확해짐)

	//UPROPERTY(EditAnywhere) // 0~1
	//float ForceScalar = 1.0f; //

	UPROPERTY(EditAnywhere)
	float CurrentHeadDegree;

	UPROPERTY(EditAnywhere)
	float HeadDegreeTurnSpeed = 30.0f;

	float TurnDirection(bool right);

	UPROPERTY(EditAnywhere) // 공이 현재 바라보는 방향으로, Z=0이고 Normalized
	FVector CurrentHeadVector;

	FVector ImpulseAmount; // 벡터로 변환된 공 발사방향

	UPROPERTY(EditAnywhere)
	FVector TorqueAmount;

	// 마그누스 효과 관련
	UPROPERTY(EditAnywhere)
	float MagnusScalar = 0.01f;

	UPROPERTY(EditAnywhere)
	float LinearDamping_Initial = 0.6f;

	UPROPERTY(EditAnywhere)
	float LinearDamping_Rolling = 0.6f;

	UPROPERTY(EditAnywhere)
	float AngularDamping_Initial = 0.05f;

	UPROPERTY(EditAnywhere)
	float AnglularDamping_Rolling = 0.05f;

	// 공의 상태: PhysicsSimulated, IsRolling, HasStopped가 차례대로 true가 되며 끝난다

	// 공 치기
	UFUNCTION()
	bool Launch(float power, float dir);

	/// 샷 이후 공이 처음으로 땅에 닿았는지 여부
	bool HasBallHitGround = false;

	// 공이 땅에 닿을 때 감속 분모
	UPROPERTY(EditAnywhere)
	float HitGroundXYVelocityDecelerationRate = 1.8f;

	UPROPERTY(EditAnywhere)
	float HitGroundZVelocityDecelerationRate = 1.0f;

	// 해당 프레임수 이상 연속으로 감속하지 않는다
	UPROPERTY(EditAnywhere)
	int HitGroundDecelerationLimitFrame = 1;

	// 해당 프레임 이상 연속으로 collision이면 구르는 상태로 판정
	UPROPERTY(EditAnywhere)
	int ConsecutiveCollisionFramesForRoll = 200;

	bool IsRolling = false;

	// 홀컵 위치
	UPROPERTY(EditAnywhere)
	class AHoleCup* HoleCup;

	// 공의 정지
	UPROPERTY(EditAnywhere)
	float StopVelocityTheshold = 35.0f;

	bool HasStopped = false;

	UPROPERTY()
	FOnBallStopped OnBallStopped;

	// 충돌 연속 체크 확인용 변수
	bool BallHitGroundLastFrame = false;
	int ConsecutiveCollision = 0; //HitGroundDecelerationLimitFrame을 넘으면 더이상 감속하지 않는다

	UFUNCTION()
	void OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// 시뮬레이션용
	UPROPERTY(EditAnywhere)
	float VisualizationCycle = 0.02f;
	FTimerHandle VisualizationHandle;
	FVector LastLocation;
	void Visualize();

	//bool fasterSimulation = false;
	FTransform InitialTransform;

	UFUNCTION()
	void ResetGolfBall();

	// 태그
	const FGameplayTagContainer& GetTagContainer() { return TagContainer; }
};