// Fill out your copyright notice in the Description page of Project Settings.


#include "BallCamera.h"

#include "BallGuide.h"
#include "GameFramework/SpringArmComponent.h"
#include "EngineUtils.h"

ABallCamera::ABallCamera(const FObjectInitializer& ObjectInitializer)
	: ACineCameraActor(ObjectInitializer)
{
	// 공 카메라 태그 붙이기
	FGameplayTag ballCameraTag = FGameplayTag::RequestGameplayTag(FName("Camera.Ball"));
	if (!TagContainer.HasTag(ballCameraTag)) { TagContainer.AddTag(ballCameraTag); }
}

void ABallCamera::BeginPlay()
{
	Super::BeginPlay();

	// 태그로 볼가이드 찾기
	ABallGuide* BallGuide = nullptr;

	for (TActorIterator<ABallGuide> It(GetWorld()); It; ++It)
	{
		if (It->GetTagContainer().HasTag(FGameplayTag::RequestGameplayTag(FName("Ball.Guide"))))
		{
			BallGuide = Cast<ABallGuide>(*It);
			break;
		}
	}

	if (BallGuide)
	{
		GetRootComponent()->AttachToComponent(BallGuide->GetSpringArmComp(), FAttachmentTransformRules::KeepRelativeTransform);

		SetActorRelativeLocation(FVector::ZeroVector);
		SetActorRelativeRotation(FRotator::ZeroRotator);
	}
	else { UE_LOG(LogTemp, Error, TEXT("공 카메라가 가 공 가이드를 찾지못함")); }
}
