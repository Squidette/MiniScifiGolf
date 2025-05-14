// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerCharacter.h"
#include "PlayerAnim.generated.h"

/**
 * 
 */
DECLARE_DELEGATE(FOnEnterAnimEnd);
DECLARE_DELEGATE(FOnShotAnimEnd);
DECLARE_DELEGATE(FOnBallHitAnim);

UENUM(BlueprintType)
enum class EShotAnims : uint8
{
	DRIVE,
	CHIP,
	PUTT
};

UCLASS()
class MINISCIFIGOLF_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	EPlayerState PlayerState;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	EShotAnims ShotAmim = EShotAnims::DRIVE;
	
	// NOTIFIES
	UFUNCTION()
	void AnimNotify_EndEnter();

	UFUNCTION()
	void AnimNotify_EndShot();

	UFUNCTION()
	void AnimNotify_BallHit();
	
public:
	void SetPlayerState(EPlayerState playerState);

	void SetShotAnim(EShotAnims shotAmim);
	
	FOnEnterAnimEnd OnEnterAnimEnd;

	FOnShotAnimEnd OnShotAnimEnd;

	FOnBallHitAnim OnBallHitAnim;
};
