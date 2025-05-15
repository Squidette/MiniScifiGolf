// Fill out your copyright notice in the Description page of Project Settings.


#include "GolfBallBase.h"
#include "EngineUtils.h"
#include "HoleCup.h"
#include "../MiniScifiGolf.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"

#define GOLFBALLBASE_RADIUS 3.0f

void AGolfBallBase::FaceHoleCup()
{
	if (!HoleCup) return;

	// 현재 방향 설정
	FacingHoleCupVector = HoleCup->GetActorLocation() - GetActorLocation();
	FacingHoleCupVector.Z = 0;
	FacingHoleCupVector.Normalize();

	// 현재 방향을 float 각도로 저장
	FRotator Rot = FacingHoleCupVector.Rotation();
	ApplyDesiredHeadingDegree(Rot.Yaw);
}

// Sets default values
AGolfBallBase::AGolfBallBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!SphereComp)
	{
		SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
		RootComponent = SphereComp;
		SphereComp->SetCollisionProfileName(TEXT("Ball"));
		SphereComp->SetNotifyRigidBodyCollision(true);
		SphereComp->InitSphereRadius(GOLFBALLBASE_RADIUS);
		SphereComp->SetUseCCD(true);
		SphereComp->SetLinearDamping(LinearDamping_Initial);
		SphereComp->SetAngularDamping(AngularDamping_Initial);
	}

	if (!StaticMeshComp)
	{
		StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
		StaticMeshComp->SetupAttachment(SphereComp);
		StaticMeshComp->SetCollisionProfileName(TEXT("NoCollision"));

		float meshScale = GOLFBALLBASE_RADIUS / 50.0f;
		StaticMeshComp->SetRelativeScale3D(FVector(meshScale, meshScale, meshScale));
	}

	if (!ArrowComp)
	{
		ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));
		ArrowComp->SetupAttachment(SphereComp);
		ArrowComp->SetCollisionProfileName(TEXT("NoCollision"));
	}

	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
		TEXT("Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (MeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(MeshAsset.Object);
	}

	// 공 태그 붙이기
	FGameplayTag ballTag = FGameplayTag::RequestGameplayTag(FName("Ball"));
	if (!TagContainer.HasTag(ballTag)) { TagContainer.AddTag(ballTag); }
}

void AGolfBallBase::ApplyDesiredHeadingDegree(float newValue)
{
	DesiredHeadingDegree = newValue;
	SetActorRotation(FRotator(0.f, DesiredHeadingDegree, 0.f));
}

void AGolfBallBase::ApplyMagnusForce(bool ignoreZ)
{
	// 마그누스 효과
	FVector MagnusForce = FVector::CrossProduct(SphereComp->GetPhysicsLinearVelocity(),
		SphereComp->GetPhysicsAngularVelocityInRadians() * MagnusScalar);
	if (ignoreZ) MagnusForce.Z = 0.0f; // 공이 물수제비 하는 것을 막아보기 위해 추가
	SphereComp->AddForce(MagnusForce);
}

void AGolfBallBase::CheckConsecutiveCollision()
{
	// 이전 프레임에 충돌했다면 카운트 올리기
	if (BallHitGroundLastFrame)
	{
		ConsecutiveCollision = FMath::Min(ConsecutiveCollision + 1, MAX_int32);
	}
	else
	{
		ConsecutiveCollision = 0;
	}
	BallHitGroundLastFrame = false;
}

FVector AGolfBallBase::CalculateFinalForce(float power, float precision)
{
	// 공이 현재 바라보는 방향에 편차를 추가
	FRotator Rot(0.f, DesiredHeadingDegree + precision * PrecisionRate, 0.0f);
	FVector finalXYDirection = Rot.Vector();

	// 힘 계산
	float launchAngleInRadians = FMath::DegreesToRadians(LaunchAngleDegree);
	float verticalComp = FMath::Sin(launchAngleInRadians);
	float horizontalComp = FMath::Cos(launchAngleInRadians);

	FVector horizontalDir = finalXYDirection * horizontalComp;
	FVector launchVelocityDir = horizontalDir;
	launchVelocityDir.Z = verticalComp;

	// 현재 골프채의 최대힘 크기와 타구바에서 전달받은 힘을 곱해서 최종 힘 벡터 계산
	return launchVelocityDir * LaunchFullForce * power; // finalforce에 어떤 연산을 가해야 선형적이어진다
}

// 에너미 FSM
void AGolfBallBase::SetState(EBallState newState)
{
	// 이미 해당 상태면 리턴
	if (newState == CurrentState) return;

	// 기존 상태에서 나갈 때 할 일
	switch (CurrentState)
	{
	case EBallState::STOPPED:
		OnExitStopped();
		break;
	default:
		CUSTOMLOG(TEXT("공의 %s상태에 지정된 EXIT함수 없음"), *UEnum::GetValueAsString(CurrentState));
		break;
	}

	// 상태 변수 바꾸기
	CurrentState = newState;
	CUSTOMLOG(TEXT("공이 %s상태 진입"), *UEnum::GetValueAsString(CurrentState));

	// 새로운 상태에 들어갈 때 할일
	switch (CurrentState)
	{
	case EBallState::STOPPED:
		OnEnterStopped();
		break;
	case EBallState::FLYING:
		OnEnterFlying();
		break;
	case EBallState::BOUNCING:
		OnEnterBouncing();
		break;
	case EBallState::ROLLING:
		OnEnterRolling();
		break;
	}
}

void AGolfBallBase::OnEnterStopped()
{
	// 바라보는 방향 초기화
	FaceHoleCup();

	// 정지한 위치 기록
	LastShotPosition = GetActorLocation();

	// 시뮬레이션 끄기
	SphereComp->SetSimulatePhysics(false);
	if (SphereComp) { SphereComp->OnComponentHit.RemoveDynamic(this, &AGolfBallBase::OnCollision); }

	// 델리게이트 실행
	if (OnBallStopped.IsBound()) { OnBallStopped.Execute(); }

	UE_LOG(LogTemp, Warning, TEXT("공 멈춤, 순간 velocity %f"), SphereComp->GetPhysicsLinearVelocity().Size());
	UE_LOG(LogTemp, Warning, TEXT("최종 거리: %f, %f, %f"), GetActorLocation().X, GetActorLocation().Y,
		GetActorLocation().Z);

	// 공 속도와 임시변수들 초기화
	SphereComp->SetPhysicsLinearVelocity(FVector(0, 0, 0));
	SphereComp->SetPhysicsAngularVelocityInRadians(FVector(0, 0, 0));
	BallHitGroundLastFrame = false;
	ConsecutiveCollision = 0;
	IsPuttingMode = false; // 퍼팅은 기본값이 아니므로 꺼준다
}

void AGolfBallBase::TickStopped()
{
}

void AGolfBallBase::OnExitStopped()
{
	// 시뮬레이션 켜기
	SphereComp->SetSimulatePhysics(true);
	if (SphereComp) { SphereComp->OnComponentHit.AddDynamic(this, &AGolfBallBase::OnCollision); }
}

void AGolfBallBase::OnEnterFlying()
{
}

void AGolfBallBase::TickFlying()
{
	ApplyMagnusForce();
}

void AGolfBallBase::OnEnterBouncing()
{
}

void AGolfBallBase::TickBouncing()
{
	ApplyMagnusForce(true);
	CheckConsecutiveCollision();
}

void AGolfBallBase::OnEnterRolling()
{
	// 구르기 진입시 Damping값을 바꾼다
	if (SphereComp)
	{
		SphereComp->SetLinearDamping(LinearDamping_Rolling);
		SphereComp->SetAngularDamping(AnglularDamping_Rolling);
	}
}

void AGolfBallBase::TickRolling()
{
	if (!IsPuttingMode) ApplyMagnusForce();
}

void AGolfBallBase::SetCurrentGroundType(UPrimitiveComponent* comp)
{
	if (comp->ComponentHasTag(TEXT("Fairway"))) { CurrentGroundType = EGroundType::FAIRWAY; }
	else if (comp->ComponentHasTag(TEXT("Green"))) { CurrentGroundType = EGroundType::GREEN; }
	else if (comp->ComponentHasTag(TEXT("Rough"))) { CurrentGroundType = EGroundType::ROUGH; }
	else if (comp->ComponentHasTag(TEXT("Bunker"))) { CurrentGroundType = EGroundType::BUNKER; }
	else if (comp->ComponentHasTag(TEXT("OB"))) { CurrentGroundType = EGroundType::OB; }
	else UE_LOG(LogTemp, Warning, TEXT("현재 지형의 태그가 파악되지 않음"));

	UE_LOG(LogTemp, Warning, TEXT("공이 %s 지형에 멈춤"), *UEnum::GetValueAsString(CurrentGroundType));
}

// Called when the game starts or when spawned
void AGolfBallBase::BeginPlay()
{
	Super::BeginPlay();

	// 시뮬레이션용
	LastLocation = GetActorLocation();
	GetWorldTimerManager().SetTimer(VisualizationHandle, this, &AGolfBallBase::Visualize, VisualizationCycle, true);

	// 태그로 홀컵 찾기
	for (TActorIterator<AHoleCup> It(GetWorld()); It; ++It)
	{
		if (It->GetTagContainer().HasTag(FGameplayTag::RequestGameplayTag(FName("HoleCup"))))
		{
			HoleCup = Cast<AHoleCup>(*It);
			break;
		}
	}

	if (!HoleCup)
	{
		CUSTOMLOG(TEXT("%s"), TEXT("홀컵 못찾음"));
	}

	// 홀컵 보게 하기
	FaceHoleCup();

	// 멈춘 상태로 초기화
	SetState(EBallState::STOPPED);
}

// Called every frame
void AGolfBallBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// FSM Tick
	switch (CurrentState)
	{
	case EBallState::STOPPED:
		TickStopped();
		break;
	case EBallState::FLYING:
		TickFlying();
		break;
	case EBallState::BOUNCING:
		TickBouncing();
		break;
	case EBallState::ROLLING:
		TickRolling();
		break;
	}

	// 디버그 스트링
	float size = SphereComp->GetPhysicsLinearVelocity().Size();
	DrawDebugString(GetWorld(), GetActorLocation(), *FString::Printf(TEXT("%.2f"), size), nullptr, FColor::Magenta, 0,
		true, 1);
}

void AGolfBallBase::TurnDirection(bool right)
{
	float changeAmount = HeadDegreeTurnSpeed * GetWorld()->GetDeltaSeconds();
	if (right) { ApplyDesiredHeadingDegree(DesiredHeadingDegree + changeAmount); }
	else { ApplyDesiredHeadingDegree(DesiredHeadingDegree - changeAmount); }
}

// 공 치기!
bool AGolfBallBase::Launch(float powerValue, float precisionValue)
{
	// 공이 멈춤 상태가 아니라면(이미 움직이는 상태라면) 리턴
	if (CurrentState != EBallState::STOPPED)
	{
		CUSTOMLOG(TEXT("%s"), TEXT("공이 이미 움직이는 중이므로 리턴"));
		return false;
	}

	// 날으는(?) 상태로 전환
	SetState(EBallState::FLYING);
	CUSTOMLOG(TEXT("Ball: Launch Ball at %f power and %f precision"), powerValue, precisionValue);

	// 전달받은 정확도를 기준으로 땅(XY)축 방향 계산
	FVector finalForce = CalculateFinalForce(powerValue, precisionValue);
	UE_LOG(LogTemp, Warning, TEXT("launched ball at finalforce of %s"), *finalForce.ToString());
	SphereComp->AddImpulse(finalForce, TEXT("None"), false);
	SphereComp->AddTorqueInRadians(TorqueAmount);

	return true;
}

bool AGolfBallBase::Putt(float powerValue, float precisionValue)
{
	// 공이 멈춤 상태가 아니라면(이미 움직이는 상태라면) 리턴
	if (CurrentState != EBallState::STOPPED)
	{
		CUSTOMLOG(TEXT("%s"), TEXT("공이 이미 움직이는 중이므로 리턴"));
		return false;
	}

	IsPuttingMode = true;

	// 공이 현재 바라보는 방향에 편차를 추가
	FRotator Rot(0.f, DesiredHeadingDegree + precisionValue * PrecisionRate, 0.0f);
	SphereComp->AddImpulse(Rot.Vector() * PuttFullforce);
	SphereComp->AddTorqueInRadians(TorqueAmount);

	return true;
}

void AGolfBallBase::Visualize()
{
	if (CurrentState != EBallState::STOPPED && LastLocation != GetActorLocation())
	{
		DrawDebugLine(GetWorld(), LastLocation, GetActorLocation(), FColor::Red, true);
	}
	LastLocation = GetActorLocation();
}

void AGolfBallBase::OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//UE_LOG(LogTemp, Warning, TEXT("AGolfBallBase::OnCollision: %s"), *OtherActor->GetActorNameOrLabel());

	if (CurrentState == EBallState::STOPPED) return;

	// 땅에 부딪힐 때마다 감속
	if (ConsecutiveCollision == 0)
	{
		FVector adjustedVel = SphereComp->GetPhysicsLinearVelocity();
		adjustedVel.X = adjustedVel.X / HitGroundXYVelocityDecelerationRate;
		adjustedVel.Y = adjustedVel.Y / HitGroundXYVelocityDecelerationRate;
		adjustedVel.Z = adjustedVel.Z / HitGroundZVelocityDecelerationRate;
		SphereComp->SetPhysicsLinearVelocity(adjustedVel);
		UE_LOG(LogTemp, Warning, TEXT("충돌로 인한 감속"));
	}

	// Flying -> Bouncing
	if (CurrentState == EBallState::FLYING)
	{
		// 비거리 측정
		UE_LOG(LogTemp, Warning, TEXT("비거리: %f, %f, %f"), Hit.Location.X, Hit.Location.Y, Hit.Location.Z);
		SetState(EBallState::BOUNCING);
	}
	// Bouncing -> Rolling
	else if (CurrentState == EBallState::BOUNCING)
	{
		// 해당 프레임 이상 땅에 닿아있었다면 Rolling 상태로 전환
		if (ConsecutiveCollision >= ConsecutiveCollisionFramesForRoll)
		{
			SetState(EBallState::ROLLING);
		}
	}
	// Rolling -> Stopped
	else if (CurrentState == EBallState::ROLLING)
	{
		// 공 멈춤 체크
		if (SphereComp->GetPhysicsLinearVelocity().Size() < StopVelocityTheshold)
		{
			SetState(EBallState::STOPPED);
			SetCurrentGroundType(OtherComp);
		}
	}

	// 공이 땅에 닿았는지 기록
	BallHitGroundLastFrame = true;
}
