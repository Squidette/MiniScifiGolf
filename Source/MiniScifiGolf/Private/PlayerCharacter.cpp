// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

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
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	auto* subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetWorld()->GetFirstPlayerController()->GetLocalPlayer());

	if (subsys) { subsys->AddMappingContext(IMC_GolfControl, 0); }
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
}

void APlayerCharacter::OnShotInput(const FInputActionValue& v)
{
	UE_LOG(LogTemp, Display, TEXT("ShotInput"));
}

void APlayerCharacter::OnMapInput(const FInputActionValue& v)
{
	UE_LOG(LogTemp, Display, TEXT("MapInput"));
}

void APlayerCharacter::OnTurnInput(const FInputActionValue& v)
{
	UE_LOG(LogTemp, Display, TEXT("TurnInput"));
}

void APlayerCharacter::OnLongerClubInput(const FInputActionValue& v)
{
	UE_LOG(LogTemp, Display, TEXT("LongerClub"));
}

void APlayerCharacter::OnShorterClubInput(const FInputActionValue& v)
{
	UE_LOG(LogTemp, Display, TEXT("ShorterClub"));
}
