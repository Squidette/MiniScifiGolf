// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"

void UPlayerAnim::AnimNotify_EndEnter()
{
	OnEnterAnimEnd.ExecuteIfBound();
}

void UPlayerAnim::SetPlayerState(EPlayerState playerState)
{
	PlayerState = playerState;
}

void UPlayerAnim::SetShotAnim(EShotAnims shotAmim)
{
	ShotAmim = shotAmim;
}