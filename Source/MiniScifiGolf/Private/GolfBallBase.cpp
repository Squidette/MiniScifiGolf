// Fill out your copyright notice in the Description page of Project Settings.


#include "GolfBallBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

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
		SphereComp->InitSphereRadius(GOLFBALLBASE_RADIUS);
		SphereComp->SetUseCCD(true);
	}

	if (!StaticMeshComp)
	{
		StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
		StaticMeshComp->SetupAttachment(SphereComp);
		StaticMeshComp->SetCollisionProfileName(TEXT("NoCollision"));
		
		float meshScale = GOLFBALLBASE_RADIUS / 50.0f;
		StaticMeshComp->SetRelativeScale3D(FVector(meshScale, meshScale, meshScale));
	}
	
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

	// 시뮬레이션용
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (!simulated && PC->WasInputKeyJustPressed(EKeys::S))
		{
			Shot();
			simulated = true;
		}

		if (PC->WasInputKeyJustPressed(EKeys::R))
		{
			ResetGolfBall();
			UE_LOG(LogTemp, Warning, TEXT("Reset"));
			simulated = false;
		}
	}

	// MAGNUS
	FVector MagnusForce = FVector::CrossProduct(SphereComp->GetPhysicsLinearVelocity(), SphereComp->GetPhysicsAngularVelocityInDegrees() * MagnusScalar);
	SphereComp->AddForce(MagnusForce);
	
}

void AGolfBallBase::Shot()
{
	SphereComp->AddImpulse(ImpulseAmount, TEXT("None"), false);
	SphereComp->AddTorqueInRadians(TorqueAmount);
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
