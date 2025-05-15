// Fill out your copyright notice in the Description page of Project Settings.


#include "BallGuide.h"

#include "EngineUtils.h"
#include "GolfBallBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ABallGuide::ABallGuide()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!SpringArmComp)
	{
		SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
		SpringArmComp->SetupAttachment(RootComponent);
	}

	// 공 가이드 태그 붙이기
	FGameplayTag tag = FGameplayTag::RequestGameplayTag(FName("Ball.Guide"));
	if (!TagContainer.HasTag(tag)) { TagContainer.AddTag(tag); }
}

// Called when the game starts or when spawned
void ABallGuide::BeginPlay()
{
	Super::BeginPlay();

	// 태그로 공 찾기
	for (TActorIterator<AGolfBallBase> It(GetWorld()); It; ++It)
	{
		if (It->GetTagContainer().HasTag(FGameplayTag::RequestGameplayTag(FName("Ball"))))
		{
			Ball = Cast<AGolfBallBase>(*It);
			break;
		}
	}

	if (Ball == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Ballguide가 공을 찾지못함"));
	}
}

// Called every frame
void ABallGuide::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 매 틱에서 공의 위치를 따라감
	SetActorLocation(Ball->GetActorLocation());

	DrawDebugLine(GetWorld(), GetActorLocation(),
	              GetActorLocation() + Ball->SphereComp->GetPhysicsLinearVelocity() * 100, FColor::Magenta, false, 0);
}
