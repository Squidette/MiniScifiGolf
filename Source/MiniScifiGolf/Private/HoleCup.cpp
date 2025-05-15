// Fill out your copyright notice in the Description page of Project Settings.


#include "HoleCup.h"
#include "Components/BoxComponent.h"

// Sets default values
AHoleCup::AHoleCup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	BoxComp->SetCollisionProfileName(TEXT("HoleCup"));

	// Ȧ�� �±� ���̱�
	FGameplayTag holeCupTag = FGameplayTag::RequestGameplayTag(FName("HoleCup"));

	if (!TagContainer.HasTag(holeCupTag))
	{
		TagContainer.AddTag(holeCupTag);
	}
}

// Called when the game starts or when spawned
void AHoleCup::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHoleCup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

