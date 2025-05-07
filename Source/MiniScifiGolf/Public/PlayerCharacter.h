// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "FieldWidget.h"
#include "PlayerCharacter.generated.h"

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	PRESHOTMOTION,
	SHOTPREP,
	SHOT,
	FLYBALL,
	POSTSHOTMOTION,
	 // ��
};

class AFieldGameMode;

UCLASS()
class MINISCIFIGOLF_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	AFieldGameMode* FieldGameModeBase;

	bool mapOpen = false;

	bool OpenMap();
	bool CloseMap();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Inputs
	UPROPERTY(EditAnywhere)
	class UInputMappingContext* IMC_GolfControl;
	
	UPROPERTY(EditAnywhere)
	class UInputAction* IA_Shot;

	UPROPERTY(EditAnywhere)
	class UInputAction* IA_Map;
	
	UPROPERTY(EditAnywhere)
	class UInputAction* IA_Turn;

	UPROPERTY(EditAnywhere)
	class UInputAction* IA_LongerClub;

	UPROPERTY(EditAnywhere)
	class UInputAction* IA_ShorterClub;

	UPROPERTY(EditAnywhere)
	class UInputAction* IA_MapHorizontal;
	
	UPROPERTY(EditAnywhere)
	class UInputAction* IA_MapVertical;
	
	void OnShotInput(const struct FInputActionValue& v);
	void OnMapInput(const struct FInputActionValue& v);
	void OnTurnInput(const struct FInputActionValue& v);
	void OnLongerClubInput(const struct FInputActionValue& v);
	void OnShorterClubInput(const struct FInputActionValue& v);
	void OnMapHorizontalInput(const struct FInputActionValue& v);
	void OnMapVerticalInput(const struct FInputActionValue& v);

	UPROPERTY(EditAnywhere)
	EPlayerState CurrentState = EPlayerState::SHOTPREP;

	UPROPERTY(EditAnywhere)
	class AGolfBallBase* Ball;

	UPROPERTY(EditAnywhere)
	class UFieldWidget* FieldWidget;

	UFUNCTION()
	void OnFieldFire(float power, float dir);
};
