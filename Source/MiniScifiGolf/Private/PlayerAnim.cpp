// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"

void UPlayerAnim::AnimNotify_EndEnter()
{
	if (OnEnterAnimEnd.IsBound())
		OnEnterAnimEnd.Execute();
}

void UPlayerAnim::AnimNotify_EndShot()
{
	if (OnShotAnimEnd.IsBound())
		OnShotAnimEnd.Execute();
}

void UPlayerAnim::AnimNotify_BallHit()
{
	if (OnBallHitAnim.IsBound())
		OnBallHitAnim.Execute();
}

void UPlayerAnim::SetPlayerState(EPlayerState playerState)
{
	PlayerState = playerState;
}

void UPlayerAnim::SetShotAnim(EShotAnims shotAmim)
{
	ShotAmim = shotAmim;
}