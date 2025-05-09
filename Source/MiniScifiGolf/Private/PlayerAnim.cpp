// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"

void UPlayerAnim::SetPlayerState(EPlayerState playerState)
{
	PlayerState = playerState;

	
}

void UPlayerAnim::SetPlayerTurning(bool b)
{
	PlayerTurning = b;
}
