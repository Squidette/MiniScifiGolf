// Fill out your copyright notice in the Description page of Project Settings.


#include "GolfBallBase.h"

#include <string>
#include "EngineUtils.h"
#include "HoleCup.h"
#include "../MiniScifiGolf.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

#define GOLFBALLBASE_RADIUS 3.0f

// Sets default values
AGolfBallBase::AGolfBallBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!SphereComp)
	{
		SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
		RootComponent = SphereComp;
		SphereComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
		SphereComp->SetSimulatePhysics(true);
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

	//if (!SpringArmComp)
	//{
	//	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	//	SpringArmComp->SetupAttachment(RootComponent);
	//}

	//// CameraComp
	//CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	//CameraComp->SetupAttachment(SpringArmComp);

	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
		TEXT("Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (MeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(MeshAsset.Object);
	}

	// 공 태그 붙이기
	FGameplayTag ballTag = FGameplayTag::RequestGameplayTag(FName("Ball"));

	if (!TagContainer.HasTag(ballTag))
	{
		TagContainer.AddTag(ballTag);
	}
}

// Called when the game starts or when spawned
void AGolfBallBase::BeginPlay()
{
	Super::BeginPlay();

	// 시뮬레이션용
	InitialTransform = GetActorTransform();
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
		CUSTOMLOG(TEXT("%s"), TEXT("Cannot find Holecup"));
	}

	//// 발사각과 힘을 벡터로 변환
	//float LaunchAngleRadians = FMath::DegreesToRadians(LaunchAngleDegree);
	//float ForceX = LaunchFullForce * FMath::Cos(LaunchAngleRadians);
	//float ForceZ = LaunchFullForce * FMath::Sin(LaunchAngleRadians);
	//ImpulseAmount =  FVector(ForceX, 0.0f, ForceZ);

	// 현재 방향 설정
	CurrentHeadVector = HoleCup->GetActorLocation() - GetActorLocation();
	CurrentHeadVector.Z = 0;
	CurrentHeadVector.Normalize();

	// 현재 방향을 float 각도로 저장
	FRotator Rot = CurrentHeadVector.Rotation();
	CurrentHeadDegree = Rot.Yaw;
	SetActorRotation(FRotator(0.f, CurrentHeadDegree, 0.f));
}

// Called every frame
void AGolfBallBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 마그누스 효과
	FVector MagnusForce = FVector::CrossProduct(SphereComp->GetPhysicsLinearVelocity(),
	                                            SphereComp->GetPhysicsAngularVelocityInRadians() * MagnusScalar);
	if (HasBallHitGround) MagnusForce.Z = 0.0f; // 땅에 구를때는 이거라도 없애줘야 그나마 자연스러워진다
	SphereComp->AddForce(MagnusForce);

	// 충돌의 연속 체크
	if (BallHitGroundLastFrame)
	{
		if (ConsecutiveCollision < FMath::Max(HitGroundDecelerationLimitFrame, ConsecutiveCollisionFramesForRoll))
		{
			ConsecutiveCollision++;
			//UE_LOG(LogTemp, Warning, TEXT("ConsecutiveCollision: %d"), ConsecutiveCollision);
		}
	}
	else
	{
		ConsecutiveCollision = 0;
	}
	BallHitGroundLastFrame = false;

	// 공 멈춤 체크
	if (IsRolling && !HasStopped && SphereComp->GetPhysicsLinearVelocity().Size() < StopVelocityTheshold)
	{
		HasStopped = true;
		SphereComp->SetPhysicsLinearVelocity(FVector(0, 0, 0));
		SphereComp->SetPhysicsAngularVelocityInRadians(FVector(0, 0, 0));
		UE_LOG(LogTemp, Warning, TEXT("공 멈춤, 순간 velocity %f"), SphereComp->GetPhysicsLinearVelocity().Size());
		UE_LOG(LogTemp, Warning, TEXT("최종 거리: %f, %f, %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
	}

	// 시뮬레이션용
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		// 샷
		if (!IsSimulated && PC->WasInputKeyJustPressed(EKeys::P))
		{
			Launch();
			IsSimulated = true;
		}

		// 리셋
		if (PC->WasInputKeyJustPressed(EKeys::R))
		{
			ResetGolfBall();
			UE_LOG(LogTemp, Warning, TEXT("Reset"));
		}

		// test
		if (PC->WasInputKeyJustPressed(EKeys::T))
		{
			//ResetGolfBallRotation();
			GetWorld()->GetDeltaSeconds();
		}
	}

	// 디버그 스트링
	float size = SphereComp->GetPhysicsLinearVelocity().Size();
	DrawDebugString(GetWorld(), GetActorLocation(), *FString::Printf(TEXT("%.2f"), size), nullptr, FColor::Magenta, 0, true, 1);
	//DrawDebugString(GetWorld(), GetActorLocation(), SphereComp->GetComponentVelocity().ToString(), nullptr, FColor::Magenta, 0, true, 2);
}

float AGolfBallBase::TurnDirection(bool right)
{
	if (right)
	{
		CurrentHeadDegree += HeadDegreeTurnSpeed * GetWorld()->GetDeltaSeconds();
	}
	else
	{
		CurrentHeadDegree -= HeadDegreeTurnSpeed * GetWorld()->GetDeltaSeconds();
	}

	SetActorRotation(FRotator(0.f, CurrentHeadDegree, 0.f));

	return 0.0f;
}

void AGolfBallBase::Launch()
{
	FRotator Rot(0.f, CurrentHeadDegree, 0.f);
	CurrentHeadVector = Rot.Vector();

	float LaunchAngleInRadians = FMath::DegreesToRadians(LaunchAngleDegree);
	float VerticalComponent = FMath::Sin(LaunchAngleInRadians);
	float HorizontalComponent = FMath::Cos(LaunchAngleInRadians);

	FVector HorizontalDirection = CurrentHeadVector * HorizontalComponent;
	FVector LaunchVelocityDirection = HorizontalDirection;
	LaunchVelocityDirection.Z = VerticalComponent;

	ImpulseAmount = LaunchVelocityDirection * LaunchFullForce;

	FVector finalForce = ImpulseAmount * ForceScalar; // finalforce에 어떤 연산을 가해야 선형적이어진다
	SphereComp->AddImpulse(finalForce, TEXT("None"), false);
	SphereComp->AddTorqueInRadians(TorqueAmount);

	if (SphereComp)
	{
		SphereComp->OnComponentHit.AddDynamic(this, &AGolfBallBase::OnCollision);
	}
}

void AGolfBallBase::Visualize()
{
	if (IsSimulated && LastLocation != GetActorLocation())
	{
		DrawDebugLine(GetWorld(), LastLocation, GetActorLocation(), FColor::Red, true);
	}
	LastLocation = GetActorLocation();
}

void AGolfBallBase::ResetGolfBall()
{
	SetActorTransform(InitialTransform);
	//SphereComp->ComponentVelocity = FVector(0, 0, 0);
	SphereComp->SetPhysicsLinearVelocity(FVector(0, 0, 0));
	SphereComp->SetPhysicsAngularVelocityInRadians(FVector(0, 0, 0));

	IsSimulated = false;
	IsRolling = false;
	HasStopped = false;
	BallHitGroundLastFrame = false;
	ConsecutiveCollision = 0;
}

void AGolfBallBase::OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsSimulated) return;

	// 처음 땅에 닿았을 때
	if (!HasBallHitGround)
	{
		// 비거리 측정
		UE_LOG(LogTemp, Warning, TEXT("비거리: %f, %f, %f"), Hit.Location.X, Hit.Location.Y, Hit.Location.Z);

		HasBallHitGround = true;
	}

	// 감속
	if (ConsecutiveCollision < HitGroundDecelerationLimitFrame)
	{
		FVector adjustedVel = SphereComp->GetPhysicsLinearVelocity();
		adjustedVel.X = adjustedVel.X / HitGroundXYVelocityDecelerationRate;
		adjustedVel.Y = adjustedVel.Y / HitGroundXYVelocityDecelerationRate;
		adjustedVel.Z = adjustedVel.Z / HitGroundZVelocityDecelerationRate;
		SphereComp->SetPhysicsLinearVelocity(adjustedVel);
	}

	// 구르는 상태 판정
	// 해당 프레임 이상 땅에 닿아있는지?
	if (!IsRolling && ConsecutiveCollision >= ConsecutiveCollisionFramesForRoll)
	{
		IsRolling = true;
		if (SphereComp)
		{
			SphereComp->SetLinearDamping(LinearDamping_Rolling);
			SphereComp->SetAngularDamping(AnglularDamping_Rolling);
		}
	}
	
	BallHitGroundLastFrame = true;
}
