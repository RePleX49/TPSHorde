// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerState.h"

// A blueprint callable function to change to Score variable
void ASPlayerState::AddScore(float ScoreDelta)
{
	Score += ScoreDelta;
}
