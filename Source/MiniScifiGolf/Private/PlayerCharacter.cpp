// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "../MiniScifiGolf.h"
#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GolfBallBase.h"
#include "FieldGameMode.h"
#include "FieldWidget.h"
#include "Kismet/GameplayStatics.h"
#include "InputMappingContext.h"
#include "PlayerAnim.h"
#include "Camera/CameraActor.h"
#include "Components/CapsuleComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 메쉬 지정
	ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMesh(
		TEXT("/Script/Engine.SkeletalMesh'/Game/Resources/Cloudia/Model/Root_Cloudia.Root_Cloudia'"));
	if (skeletalMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(skeletalMesh.Object);
	}

	// 애니메이션 블루프린트 지정
	ConstructorHelpers::FObjectFinder<UAnimBlueprint> animBlueprint(
		TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/ABP_PlayerRootAnim.ABP_PlayerRootAnim'"));
	if (animBlueprint.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(animBlueprint.Object->GeneratedClass);
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

	// 충돌 설정
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
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
			
			FieldWidget->OnShotBarEnded.BindUObject(this, &APlayerCharacter::OnShotMade);
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
		Ball->OnBallStopped.BindUObject(this, &APlayerCharacter::OnBallStopped);
	}

	// 애니메이션 블루프린트 저장
	if (UAnimInstance* animInst = GetMesh()->GetAnimInstance())
	{
		Anim = Cast<UPlayerAnim>(animInst);

		if (Anim)
		{
			Anim->OnEnterAnimEnd.BindUObject(this, &APlayerCharacter::OnEnterAnimationEnd);
			Anim->OnShotAnimEnd.BindUObject(this, &APlayerCharacter::OnShotAnimationEnd);
			Anim->OnBallHitAnim.BindUObject(this, &APlayerCharacter::OnBallHitAnimation);
		}
	}

	SetCurrentState(EPlayerState::PRESHOTMOTION);	
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (FieldWidget) { FieldWidget->OnShotBarEnded.Unbind(); }
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
	if (CurrentState != EPlayerState::SHOTPREP && CurrentState != EPlayerState::SHOTBARRUN) return;

	// 맵 보는 중 샷을 하려고 하면 맵에서 나간다
	if (mapOpen)
	{
		CloseMap();
		return;
	}

	if (FieldWidget)
	{
		bool isActivated = FieldWidget->PressShotBar();

		if (isActivated)
		{
			SetCurrentState(EPlayerState::SHOTBARRUN);
		}
	}
}

void APlayerCharacter::OnMapInput(const FInputActionValue& v)
{
	if (CurrentState != EPlayerState::SHOTPREP) return;
	UE_LOG(LogTemp, Display, TEXT("MapInput"));

	// 이미 타구바가 움직이는 중이면 리턴한다
	if (FieldWidget && FieldWidget->GetState() != EShotBarState::INACTIVE) return;

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

	if (FieldGameModeBase)
	{
		FieldGameModeBase->MoveMapCameraHorizontal(v.Get<float>());
	}
}

void APlayerCharacter::OnMapVerticalInput(const struct FInputActionValue& v)
{
	if (CurrentState != EPlayerState::SHOTPREP) return;
	if (!mapOpen) return;

	if (FieldGameModeBase)
	{
		FieldGameModeBase->MoveMapCameraVertical(v.Get<float>());
	}
}

// 공이 멈췄을 때
void APlayerCharacter::OnBallStopped()
{
	// 플레이어 리셋
	SetCurrentState(EPlayerState::PRESHOTMOTION);

	// 카메라 변경
	if (FieldGameModeBase)
	{
		FieldGameModeBase->SetCameraModeWithBlend(ECameraMode::PLAYER);
	}
}

void APlayerCharacter::AttachPlayerAndCameraToBall()
{
	// 이미 붙어있다면 리턴한다
	if (IsPlayerAndCameraAttachedToBall == true) return;
	
	// 플레이어를 공에 붙인다
	USceneComponent* BallRootComp = Cast<USceneComponent>(Ball->GetRootComponent());
	GetRootComponent()->AttachToComponent(BallRootComp, FAttachmentTransformRules::KeepRelativeTransform);
	GetRootComponent()->SetRelativeLocation(PlayerOffsetFromBall);
	GetRootComponent()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	// 카메라도 붙인다
	if (FieldGameModeBase)
	{
		FieldGameModeBase->GetPlayerCamera()->GetRootComponent()->AttachToComponent(
			BallRootComp, FAttachmentTransformRules::KeepRelativeTransform);
		FieldGameModeBase->GetPlayerCamera()->GetRootComponent()->SetRelativeLocation(PlayerCameraOffsetFromBall);
		FieldGameModeBase->GetPlayerCamera()->GetRootComponent()->SetRelativeRotation(FRotator::ZeroRotator);
	}
	
	IsPlayerAndCameraAttachedToBall = true;

	CUSTOMLOG(TEXT("플레이어와 카메라를 공에 붙입니다"), *UEnum::GetValueAsString(CurrentState));
}

void APlayerCharacter::DettachPlayerAndCameraFromBall()
{
	// 이미 떨어져 있다면 리턴한다
	if (IsPlayerAndCameraAttachedToBall == false) return;

	// 공에서 캐릭터와 카메라를 뗀다
	GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	if (FieldGameModeBase)
	{
		FieldGameModeBase->GetPlayerCamera()->GetRootComponent()->DetachFromComponent(
			FDetachmentTransformRules::KeepWorldTransform);
	}

	IsPlayerAndCameraAttachedToBall = false;

	CUSTOMLOG(TEXT("플레이어와 카메라를 공에서 뗍니다"), *UEnum::GetValueAsString(CurrentState));
}

void APlayerCharacter::PlacePlayerAndCameraAroundBall()
{
	FTransform ballTransform = Ball->GetActorTransform();
	GetRootComponent()->SetWorldLocation(ballTransform.TransformPosition(PlayerOffsetFromBall));
	if (FieldGameModeBase)
	{
		FieldGameModeBase->GetPlayerCamera()->GetRootComponent()->SetWorldLocation(ballTransform.TransformPosition(PlayerCameraOffsetFromBall));
	}
}

void APlayerCharacter::OnShotMade(bool success, float power, float dir)
{
	// 샷 모션 상태로 넘어감
	SetCurrentState(EPlayerState::SHOT);

	TempSuccess = success;
	TempPower = power;
	TempDir = dir;

}

bool APlayerCharacter::OpenMap()
{
	if (mapOpen) return false;

	if (FieldGameModeBase)
	{
		FieldGameModeBase->SetCameraMode(ECameraMode::MAP);
	}

	mapOpen = true;

	return true;
}

bool APlayerCharacter::CloseMap()
{
	if (!mapOpen) return false;

	if (FieldGameModeBase)
	{
		FieldGameModeBase->SetCameraModeWithBlend(ECameraMode::PLAYER);
	}

	mapOpen = false;

	return true;
}

void APlayerCharacter::ShotBall()
{
	// 타구바 인풋 실패시 실수 샷으로 설정
	if (!TempSuccess)
	{
		// 랜덤한 작은 파워로, 랜덤한 방향으로 치게된다
		TempPower = FMath::RandRange(0.01f, 0.2f);
		TempDir = FMath::RandRange(-1.0f, 1.0f);
		CUSTOMLOG(TEXT("실패 샷 - 랜덤한 %f의 힘, %f의 정확도로 설정되어 Ball->Launch"), TempPower, TempDir);
	}
	else CUSTOMLOG(TEXT("타구바에서 전달된 %f의 힘, %f의 정확도로 Ball->Launch"), TempPower, TempDir);

	if (Ball->Launch(TempPower, TempDir))
	{
		SetCurrentState(EPlayerState::FLYBALL);
		FieldGameModeBase->SetCameraModeWithBlend(ECameraMode::BALL);
	}
	else
	{
		CUSTOMLOG(TEXT("Ball Launch 실패함"), TempPower, TempDir);
	}
}

void APlayerCharacter::SetCurrentState(EPlayerState newState)
{
	// 이미 해당 당태면 리턴
	if (CurrentState == newState) return;

	CurrentState = newState;
	
	// enter
	switch (CurrentState)
	{
	case EPlayerState::PRESHOTMOTION:
		PlacePlayerAndCameraAroundBall();
		break;
	case EPlayerState::SHOTPREP:
		AttachPlayerAndCameraToBall();
		break;
	case EPlayerState::FLYBALL:
		DettachPlayerAndCameraFromBall();
		break;
	default:
		break;
	}
	
	// 애니메이션에도 상태변화 전달
	if (Anim)
	{
		Anim->SetPlayerState(CurrentState);
	}

	CUSTOMLOG(TEXT("플레이어 상태 %s로 변화"), *UEnum::GetValueAsString(CurrentState));
}

void APlayerCharacter::OnEnterAnimationEnd()
{
	SetCurrentState(EPlayerState::SHOTPREP);
}

void APlayerCharacter::OnShotAnimationEnd()
{
}

void APlayerCharacter::OnBallHitAnimation()
{
	SetCurrentState(EPlayerState::FLYBALL);
	ShotBall();
}
