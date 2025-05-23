// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	PRESHOTMOTION,
	SHOTPREP,
	SHOTBARRUN,
	SHOT,
	FLYBALL,
	POSTSHOTMOTION,
	 // 끝
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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// 게임모드 베이스에 대한 포인터
	UPROPERTY()
	class AFieldGameMode* FieldGameModeBase = nullptr;

	// 타구바 위젯 포인터
	UPROPERTY()
	class UFieldWidget* FieldWidget = nullptr;

	// 공에 대한 포인터
	UPROPERTY()
	class AGolfBallBase* Ball;

	// 애니메이션 블루프린트
	UPROPERTY()
	class UPlayerAnim* Anim;

	// 플레이어의 현재 상태
	UPROPERTY(VisibleAnywhere)
	EPlayerState CurrentState = EPlayerState::POSTSHOTMOTION;
	
	// 등장 애니메이션이 끝나면 ABP에서 호출
	void OnEnterAnimationEnd();

	// 샷 애니메이션이 끝나면 ABP에서 호출
	void OnShotAnimationEnd();

	// 샷 애니메이션이 공을 치는 순간 호출
	void OnBallHitAnimation();

	// 타구바에서 샷이 결정되면 타구바 UI에서 호출
	void OnShotMade(bool success, float power, float dir);
	
	// 공이 멈추면 공 클래스에서 호출
	void OnBallStopped();
	
	// 맵
	bool mapOpen = false;
	bool OpenMap();
	bool CloseMap();
	
	// 공 치기
	void ShotBall();

	// 공 치는데 들어갈 값
	bool TempSuccess;
	float TempPower;
	float TempDir;

	// 플레이어와 카메라가 공의 트랜스폼에 붙어 있는지 여부
	bool IsPlayerAndCameraAttachedToBall = false;
	void AttachPlayerAndCameraToBall();
	void DettachPlayerAndCameraFromBall();
	
	// 플레이어가 공으로부터 얼마나 떨어져있을 것인지
	UPROPERTY(EditDefaultsOnly)
	FVector PlayerOffsetFromBall = FVector(0.0f, -70.0f, 100.0f);

	// 플레이어 카메라가 공으로부터 얼마나 떨어져있을 것인지
	UPROPERTY(EditDefaultsOnly)
	FVector PlayerCameraOffsetFromBall = FVector(-400.0f, 0.0f, 110.0f);

	// 홀컵의 위치를 고려하여 플레이어와 카메라 포지션 리셋
	void PlacePlayerAndCameraAroundBall();

	// 인풋
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

	void SetCurrentState(EPlayerState newState);

public:
	
};
