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

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}
		
	if (!SphereComp)
	{
		SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
		SphereComp->InitSphereRadius(GOLFBALLBASE_RADIUS);
		RootComponent = SphereComp;
	}

	if (!StaticMeshComp)
	{
		StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
		StaticMeshComp->SetupAttachment(SphereComp);
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

	
	
}

// Called every frame
void AGolfBallBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->WasInputKeyJustPressed(EKeys::Q))
		{
			UE_LOG(LogTemp, Warning, TEXT("Q was just pressed!"));
		}
	}

	
}

void AGolfBallBase::Shot()
{
	
}