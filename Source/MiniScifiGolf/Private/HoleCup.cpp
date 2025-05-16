// Fill out your copyright notice in the Description page of Project Settings.


#include "HoleCup.h"

#include "FieldGameMode.h"
#include "GolfBallBase.h"
#include "ResultWidget.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AHoleCup::AHoleCup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);
	BoxComp->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	BoxComp->SetCollisionProfileName(TEXT("HoleCup"));

	// 홀컵 태그 붙이기
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

	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AHoleCup::OnBallCollision);
}

// Called every frame
void AHoleCup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHoleCup::OnBallCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 볼 태그가 있는지 확인
	//if (OtherActor)

	AGolfBallBase* golfBall = Cast<AGolfBallBase>(OtherActor);
	if (golfBall)
	{
		UE_LOG(LogTemp, Warning, TEXT("골프공이 닿았습니다"));
		auto* gm = Cast<AFieldGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (gm) gm->ResultWidget->Show();
	}
}
