// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "../MiniScifiGolf.h"
#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GolfBallBase.h"
#include "FieldGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "InputMappingContext.h"
#include "MovieSceneTracksComponentTypes.h"
#include "Camera/CameraActor.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 메쉬 지정
	ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMesh(
		TEXT("/Script/Engine.SkeletalMesh'/Game/Resources/Cloudia/Model/Cloudia_Mesh.Cloudia_Mesh'"));
	if (skeletalMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(skeletalMesh.Object);
	}

	ConstructorHelpers::FObjectFinder<UInputMappingContext> imc(
		TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Inputs/IMC_GolfControl.IMC_GolfControl'"));
	if (imc.Succeeded()) IMC_GolfControl = imc.Object;

	ConstructorHelpers::FObjectFinder<UInputAction> shotInputAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Inputs/IA_Shot.IA_Shot'"));
	if (shotInputAction.Succeeded()) IA_Shot = shotInputAction.Object;

	ConstructorHelpers::FObjectFinder<UInputAction> mapInputAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Inputs/IA_Map.IA_Map'"));
	if (mapInputAction.Succeeded()) IA_Map = mapInputAction.Object;

	ConstructorHelpers::FObjectFinder<UInputAction> turnInputAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Inputs/IA_Turn.IA_Turn'"));
	if (turnInputAction.Succeeded()) IA_Turn = turnInputAction.Object;

	ConstructorHelpers::FObjectFinder<UInputAction> longerClubInputAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Inputs/IA_LongerClub.IA_LongerClub'"));
	if (longerClubInputAction.Succeeded()) IA_LongerClub = longerClubInputAction.Object;

	ConstructorHelpers::FObjectFinder<UInputAction> shorterClubInputAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Inputs/IA_ShorterClub.IA_ShorterClub'"));
	if (shorterClubInputAction.Succeeded()) IA_ShorterClub = shorterClubInputAction.Object;

	ConstructorHelpers::FObjectFinder<UInputAction> mapHorizontalInputAction(
	TEXT("/Script/EnhancedInput.InputAction'/Game/Inputs/IA_MapHorizontal.IA_MapHorizontal'"));
	if (mapHorizontalInputAction.Succeeded()) IA_MapHorizontal = mapHorizontalInputAction.Object;

	ConstructorHelpers::FObjectFinder<UInputAction> mapVerticalInputAction(
	TEXT("/Script/EnhancedInput.InputAction'/Game/Inputs/IA_MapVertical.IA_MapVertical'"));
	if (mapVerticalInputAction.Succeeded()) IA_MapVertical = mapVerticalInputAction.Object;

	bUseControllerRotationYaw = false;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	auto* subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetWorld()->GetFirstPlayerController()->GetLocalPlayer());
	
	if (subsys) { subsys->AddMappingContext(IMC_GolfControl, 0); }

	// FieldGameMode와 FieldWidget 포인터 가져오기
	AGameModeBase* gmb = UGameplayStatics::GetGameMode(GetWorld());
	if (gmb)
	{
		FieldGameModeBase = Cast<AFieldGameMode>(gmb);

		if (FieldGameModeBase)
		{
			FieldWidget = FieldGameModeBase->FieldWidget;

			FieldWidget->OnShotMade.BindDynamic(this, &APlayerCharacter::OnFieldFire);
		}
	}

	// 태그로 공 찾기
	for (TActorIterator<AGolfBallBase> It(GetWorld()); It; ++It)
	{
		if (It->GetTagContainer().HasTag(FGameplayTag::RequestGameplayTag(FName("Ball"))))
		{
			Ball = Cast<AGolfBallBase>(*It);
			break;
		}
	}

	if (!Ball)
	{
		CUSTOMLOG(TEXT("%s"), TEXT("Player Cannot find ball"));
	}
	else
	{
		Ball->OnBallStopped.BindDynamic(this, &APlayerCharacter::OnBallStopped);

		// 플레이어를 공에 붙인다
		USceneComponent* BallRootComp = Cast<USceneComponent>(Ball->GetRootComponent());
		GetRootComponent()->AttachToComponent(BallRootComp, FAttachmentTransformRules::KeepRelativeTransform);
		GetRootComponent()->SetRelativeLocation(PlayerOffsetFromBall);

		// 카메라도 붙인다
		FieldGameModeBase->GetPlayerCamera()->GetRootComponent()->AttachToComponent(BallRootComp, FAttachmentTransformRules::KeepRelativeTransform);
		FieldGameModeBase->GetPlayerCamera()->GetRootComponent()->SetRelativeLocation(PlayerCameraOffsetFromBall);
	}
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (FieldWidget) { FieldWidget->OnShotMade.Unbind(); }
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (IA_Shot)
	{
		Input->BindAction(IA_Shot, ETriggerEvent::Started, this, &APlayerCharacter::OnShotInput);
	}
	if (IA_Map)
	{
		Input->BindAction(IA_Map, ETriggerEvent::Started, this, &APlayerCharacter::OnMapInput);
	}
	if (IA_Turn)
	{
		Input->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &APlayerCharacter::OnTurnInput);
	}
	if (IA_LongerClub)
	{
		Input->BindAction(IA_LongerClub, ETriggerEvent::Started, this, &APlayerCharacter::OnLongerClubInput);
	}
	if (IA_ShorterClub)
	{
		Input->BindAction(IA_ShorterClub, ETriggerEvent::Started, this, &APlayerCharacter::OnShorterClubInput);
	}
	if (IA_MapHorizontal)
	{
		Input->BindAction(IA_MapHorizontal, ETriggerEvent::Triggered, this, &APlayerCharacter::OnMapHorizontalInput);
	}
	if (IA_MapVertical)
	{
		Input->BindAction(IA_MapVertical, ETriggerEvent::Triggered, this, &APlayerCharacter::OnMapVerticalInput);
	}
	
}

void APlayerCharacter::OnShotInput(const FInputActionValue& v)
{
	if (CurrentState != EPlayerState::SHOTPREP) return;
	UE_LOG(LogTemp, Display, TEXT("ShotInput"));

	// 맵 보는 중 샷을 하려고 하면 맵에서 나간다
	if (mapOpen)
	{
		CloseMap();
		return;
	}
	
	if (FieldWidget)
	{
		FieldWidget->PressShotBar();
	}
}

void APlayerCharacter::OnMapInput(const FInputActionValue& v)
{
	if (CurrentState != EPlayerState::SHOTPREP) return;
	UE_LOG(LogTemp, Display, TEXT("MapInput"));

	// 이미 타구바가 움직이는 중이면 리턴한다
	if (FieldWidget && FieldWidget->GetShotBarActivated()) return;
	
	if (mapOpen) CloseMap();
	else OpenMap();
}

void APlayerCharacter::OnTurnInput(const FInputActionValue& v)
{
	if (CurrentState != EPlayerState::SHOTPREP) return;
	//UE_LOG(LogTemp, Display, TEXT("TurnInput %f"), v.Get<float>());

	if (Ball)
	{
		if (v.Get<float>() > 0)
		{
			Ball->TurnDirection(true);
		}
		else
		{
			Ball->TurnDirection(false);
		}
	}
}

void APlayerCharacter::OnLongerClubInput(const FInputActionValue& v)
{
	if (CurrentState != EPlayerState::SHOTPREP) return;
	UE_LOG(LogTemp, Display, TEXT("LongerClub"));
}

void APlayerCharacter::OnShorterClubInput(const FInputActionValue& v)
{
	if (CurrentState != EPlayerState::SHOTPREP) return;
	UE_LOG(LogTemp, Display, TEXT("ShorterClub"));
}

void APlayerCharacter::OnMapHorizontalInput(const struct FInputActionValue& v)
{
	if (CurrentState != EPlayerState::SHOTPREP) return;
	if (!mapOpen) return;

	FieldGameModeBase->MoveMapCameraHorizontal(v.Get<float>());
}

void APlayerCharacter::OnMapVerticalInput(const struct FInputActionValue& v)
{
	if (CurrentState != EPlayerState::SHOTPREP) return;
	if (!mapOpen) return;

	FieldGameModeBase->MoveMapCameraVertical(v.Get<float>());
}

// 공이 멈췄을 때
void APlayerCharacter::OnBallStopped()
{
	// 플레이어가 공 쪽으로 간다
	SetActorLocation(Ball->GetActorLocation() + PlayerOffsetFromBall);

	// 카메라 변경
	FieldGameModeBase->SetCameraModeWithBlend(ECameraMode::PLAYER);
}

// 위젯에서 샷이 확정되었을 때 정보가 보내짐
void APlayerCharacter::OnFieldFire(float power, float dir)
{
	// 공에서 캐릭터와 카메라를 뗀다
	GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	FieldGameModeBase->GetPlayerCamera()->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	
	if (!Ball->Launch(power, dir))
	{
		CUSTOMLOG(TEXT("%s"), TEXT("Ball Launch fail"));
	}
	else
	{
		CurrentState = EPlayerState::FLYBALL; // 모션 넣으면 SHOT으로 바꾸자.. 임시로 FLYBALL
		FieldGameModeBase->SetCameraModeWithBlend(ECameraMode::BALL);
	}
}

bool APlayerCharacter::OpenMap()
{
	if (mapOpen) return false;

	FieldGameModeBase->SetCameraMode(ECameraMode::MAP);

	mapOpen = true;
	
	return true;
}

bool APlayerCharacter::CloseMap()
{
	if (!mapOpen) return false;

	FieldGameModeBase->SetCameraModeWithBlend(ECameraMode::PLAYER);

	mapOpen = false;
	
	return true;
}
