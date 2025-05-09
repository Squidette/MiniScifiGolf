// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerCharacter.h"
#include "PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class MINISCIFIGOLF_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPlayerState PlayerState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool PlayerTurning;

public:
	void SetPlayerState(EPlayerState playerState);
	void SetPlayerTurning(bool b);
};
