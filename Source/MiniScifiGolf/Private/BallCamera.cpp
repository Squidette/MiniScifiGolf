// Fill out your copyright notice in the Description page of Project Settings.


#include "BallCamera.h"
#include "GolfBallBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "EngineUtils.h"

ABallCamera::ABallCamera(const FObjectInitializer& ObjectInitializer)
	: ACineCameraActor(ObjectInitializer)
{
	// 공 카메라 태그 붙이기
	FGameplayTag ballCameraTag = FGameplayTag::RequestGameplayTag(FName("Camera.Ball"));

	if (!TagContainer.HasTag(ballCameraTag))
	{
		TagContainer.AddTag(ballCameraTag);
	}
}

void ABallCamera::BeginPlay()
{
	Super::BeginPlay();

	// 태그로 공 찾기
	AGolfBallBase* Ball = nullptr;

	for (TActorIterator<AGolfBallBase> It(GetWorld()); It; ++It)
	{
		if (It->GetTagContainer().HasTag(FGameplayTag::RequestGameplayTag(FName("Ball"))))
		{
			Ball = Cast<AGolfBallBase>(*It);
			break;
		}
	}

	//공을 찾았다면 공의 SpringArm에 붙기
	if (Ball)
	{
		if (Ball->SpringArmComp)
		{
			USceneComponent* BallSpringArm = Cast<USceneComponent>(Ball->SpringArmComp);

			GetRootComponent()->AttachToComponent(BallSpringArm, FAttachmentTransformRules::KeepRelativeTransform);

			SetActorRelativeLocation(FVector::ZeroVector);
			SetActorRelativeRotation(FRotator::ZeroRotator);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BallCamera can't find ball actor"));
	}
}