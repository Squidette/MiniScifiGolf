// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GolfBallBase.generated.h"

UENUM(BlueprintType)
enum class EBallState : uint8
{
	STOPPED,
	FLYING,
	BOUNCING,
	ROLLING
};

UENUM(BlueprintType)
enum class EGroundType : uint8
{
	FAIRWAY,
	ROUGH,
	BUNKER,
	GREEN,
	OB
};

DECLARE_DELEGATE(FOnBallStopped);

UCLASS()
class MINISCIFIGOLF_API AGolfBallBase : public AActor
{
	GENERATED_BODY()

	// 태그
	UPROPERTY()
	FGameplayTagContainer TagContainer;

	/// [FSM] ------------------------------------------------------------------------
	UPROPERTY(VisibleAnywhere)
	EBallState CurrentState = EBallState::ROLLING; // STOPPED가 아닌 아무거나로 초기화
	void SetState(EBallState newState);

	// 공이 멈춰있는 상태
	void OnEnterStopped();
	void TickStopped();
	void OnExitStopped();

	// 공이 날아가는 상태
	void OnEnterFlying();
	void TickFlying();

	// 공이 땅에 떨어진 후 튀기는 상태
	void OnEnterBouncing();
	void TickBouncing();

	// 공이 땅에서 굴러가고 있는 상태
	void OnEnterRolling();
	void TickRolling();
	///-------------------------------------------------------------------------------
	

	/// [샷 준비시 공 컨트롤 관련] -----------------------------------------------------
	// 공 좌우로 돌리는 속도
	UPROPERTY(EditDefaultsOnly)
	float HeadDegreeTurnSpeed = 30.0f;

	// 현재 공의 앞방향 (XY축)
	float CurrentYawDegree;
	void UpdateCurrentYawDegree(float newValue);

	// 공이 홀컵을 바라보게 설정
	void FaceHoleCup();

	// 마지막 샷 위치 기록
	FVector LastShotPosition = FVector::ZeroVector;
	///-------------------------------------------------------------------------------


	/// [샷 시작시 필요한 것들] --------------------------------------------------------
	// 현재 가진 정보를 바탕으로 골프공의 최종 힘 계산
	FVector CalculateFinalForce(float power, float precision);

	// 퍼팅 관련 변수들
	UPROPERTY(EditAnywhere, Category = "Putt")
	float PuttFullforce = 900.0f;

	bool IsPuttingMode = false;
	///-------------------------------------------------------------------------------


	/// [샷 도중 필요한 것들] ----------------------------------------------------------
	// 충돌이 일정 프레임 이상 연속으로 들어왔는지 체크
	void CheckConsecutiveCollision();
	
	// 공이 현재 위치한 지형
	EGroundType CurrentGroundType = EGroundType::ROUGH;
	void SetCurrentGroundType(UPrimitiveComponent* hitGroundComp); // 공이 멈추는 순간 실행
	
	// 마그누스 효과
	UPROPERTY(EditAnywhere)
	bool bApplyMagnusForce = true;
	
	UPROPERTY(EditAnywhere)
	float MagnusScalar = 0.01f;
	void ApplyMagnusForce(bool ignoreZ = false);

	// 공 상태별 Damping값
	UPROPERTY(EditAnywhere)
	float LinearDamping_InAir = 0.6f;

	UPROPERTY(EditAnywhere)
	float AngularDamping_InAir = 0.05f;

	UPROPERTY(EditAnywhere)
	float LinearDamping_Rolling = 0.6f;

	UPROPERTY(EditAnywhere)
	float AngularDamping_Rolling = 0.05f;

	UPROPERTY(EditAnywhere)
	float LinearDamping_Putt = 0.1f;

	UPROPERTY(EditAnywhere)
	float AngularDamping_Putt = 0.1f;
	///-------------------------------------------------------------------------------

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	// Sets default values for this actor's properties
	AGolfBallBase();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// 기본 컴포넌트
	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereComp;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere)
	class UArrowComponent* ArrowComp;

	// 공 세팅시 정해지는 수치 (타구바에 따라 최종수치는 달라질수 있음)
	UPROPERTY(EditAnywhere) // 공이 땅과 이루는 발사각, 0~90사이
	float LaunchAngleDegree = 28.0f;

	UPROPERTY(EditAnywhere)
	float LaunchFullForce = 950.0f; // 100% 힘의 세기로, 채 종류에 의해 결정

	UPROPERTY(EditAnywhere)
	float PrecisionRate = 5.0f; // 샷의 정확도 (양수, 0에 가까워질수록 정확해짐)

	UPROPERTY(EditAnywhere)
	FVector TorqueAmount;
	
	// 공이 땅에 닿을 때 감속 분모
	UPROPERTY(EditAnywhere)
	float HitGroundXYVelocityDecelerationRate = 1.8f;

	UPROPERTY(EditAnywhere)
	float HitGroundZVelocityDecelerationRate = 1.0f;

	// 해당 프레임 이상 연속충돌하면 구르는 상태로 판정
	UPROPERTY(EditAnywhere)
	int ConsecutiveCollisionFramesForRoll = 200;

	// 홀컵 위치
	UPROPERTY(EditAnywhere)
	class AHoleCup* HoleCup;

	// 속도가 이 값 아래로 내려가면 정지
	UPROPERTY(EditAnywhere)
	float StopVelocityThreshold = 55.0f;

	// 속도가 이 값 아래로 내려가면 정지
	UPROPERTY(EditAnywhere)
	float StopVelocityThreshold_Putt = 55.0f;

	//bool HasStopped = false;

	FOnBallStopped OnBallStopped;

	// 충돌 연속 체크 확인용 변수
	bool BallHitGroundLastFrame = false; // 이전 프레임에서 충돌했는지
	int32 ConsecutiveCollision = 0; // 얼마나 연속으로 충돌했는지

	UFUNCTION()
	void OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// 시뮬레이션용
	UPROPERTY(EditAnywhere)
	float VisualizationCycle = 0.02f;
	FTimerHandle VisualizationHandle;
	FVector LastLocation;
	void Visualize();

	// 태그
	const FGameplayTagContainer& GetTagContainer() { return TagContainer; }

	inline EGroundType GetCurrentGroundType() const { return CurrentGroundType; }

	// 일반 공 치기!
	bool Launch(float powerValue, float precisionValue);

	// 퍼팅!
	bool Putt(float powerValue, float precisionValue);

	// 공 방향 돌리기
	void TurnDirection(bool right);
};