// Fill out your copyright notice in the Description page of Project Settings.


#include "GolfBallBase.h"

#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
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
		SphereComp->SetLinearDamping(LinearDamping_InAir);
		SphereComp->SetAngularDamping(AngularDamping_InAir);
	}

	if (!StaticMeshComp)
	{
		StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
		StaticMeshComp->SetupAttachment(SphereComp);
		StaticMeshComp->SetCollisionProfileName(TEXT("NoCollision"));
		
		float meshScale = GOLFBALLBASE_RADIUS / 50.0f;
		StaticMeshComp->SetRelativeScale3D(FVector(meshScale, meshScale, meshScale));
	}

	if (!SpringArmComp)
	{
		SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
		SpringArmComp->SetupAttachment(RootComponent);
	}

	// CameraComp
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);
	
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (MeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(MeshAsset.Object);
	}
}

// Called when the game starts or when spawned
void AGolfBallBase::BeginPlay()
{
	Super::BeginPlay();

	InitialTransform = GetActorTransform();

	LastLocation = GetActorLocation();
	GetWorldTimerManager().SetTimer(VisualizationHandle, this, &AGolfBallBase::Visualize, VisualizationCycle, true);
}

// Called every frame
void AGolfBallBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 마그누스 효과
	FVector MagnusForce = FVector::CrossProduct(SphereComp->GetPhysicsLinearVelocity(), SphereComp->GetPhysicsAngularVelocityInRadians() * MagnusScalar);
	if (HitGround) MagnusForce.Z = 0.0f; // 땅에 구를때는 이거라도 없애줘야 그나마 자연스러워진다
	SphereComp->AddForce(MagnusForce);

	
	// 시뮬레이션용
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		// 샷
		if (!simulated && PC->WasInputKeyJustPressed(EKeys::P))
		{
			Shot();
			simulated = true;
		}

		// 리셋
		if (PC->WasInputKeyJustPressed(EKeys::R))
		{
			ResetGolfBall();
			UE_LOG(LogTemp, Warning, TEXT("Reset"));
			simulated = false;
		}

		// 시간 가속
		if (PC->WasInputKeyJustPressed(EKeys::T))
		{
			if (fasterSimulation)
			{
				UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
				fasterSimulation = false;
			}
			else
			{
				UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 6.0f);
				fasterSimulation = true;
			}
		}

		//
		if (PC->WasInputKeyJustPressed(EKeys::NumPadOne))
		{
			// Camera 1
		}
	}

	DrawDebugString(GetWorld(), GetActorLocation(), MagnusForce.ToString(), nullptr, FColor::Magenta, 0, true, 2);
}

void AGolfBallBase::Shot()
{
	SphereComp->AddImpulse(ImpulseAmount, TEXT("None"), false);
	SphereComp->AddTorqueInRadians(TorqueAmount);

	if (SphereComp)
	{
		SphereComp->OnComponentHit.AddDynamic(this, &AGolfBallBase::OnFirstCollision);
	}
}

void AGolfBallBase::Visualize()
{
	if (LastLocation != GetActorLocation())
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
}

void AGolfBallBase::OnFirstCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HitGround)
	{
		// 비거리 측정
		UE_LOG(LogTemp, Warning, TEXT("collision: %f, %f, %f"), Hit.Location.X, Hit.Location.Y, Hit.Location.Z);

		if (SphereComp)
		{
			SphereComp->SetLinearDamping(LinearDamping_AfterGroundHit);
			SphereComp->SetAngularDamping(AnglularDamping_AfterGroundHit);
		}
		
		HitGround = true;
	}
}
