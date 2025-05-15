// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundBase.h"

// Sets default values
AGroundBase::AGroundBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(StaticMeshComponent);
	StaticMeshComponent->SetCollisionProfileName(TEXT("Ground"));
	// ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(
	// 	TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));
	//
	// if (tempMesh.Succeeded())
	// {
	// 	StaticMeshComponent->SetStaticMesh(tempMesh.Object);
	// }
}

// Called when the game starts or when spawned
void AGroundBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGroundBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}