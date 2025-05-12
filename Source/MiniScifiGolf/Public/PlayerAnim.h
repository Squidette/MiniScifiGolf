// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerCharacter.h"
#include "PlayerAnim.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_DELEGATE(FOnEnterAnimEnd);
DECLARE_DYNAMIC_DELEGATE(FOnShotAnimEnd);

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPlayerState PlayerState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EShotAnims ShotAmim = EShotAnims::DRIVE;
	
	// NOTIFIES
	UFUNCTION()
	void AnimNotify_EndEnter();

	UFUNCTION()
	void AnimNotify_EndShot();

public:
	void SetPlayerState(EPlayerState playerState);

	void SetShotAnim(EShotAnims shotAmim);
	
	UPROPERTY()
	FOnEnterAnimEnd OnEnterAnimEnd;

	UPROPERTY()
	FOnShotAnimEnd OnShotAnimEnd;
};
