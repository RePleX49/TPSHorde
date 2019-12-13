// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TPSHORDE_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	FTimerHandle TimerHandle_BotSpawner;
	FTimerHandle TimerHandle_NextWaveStart;

	// number of bots to spawn in a wave
	int32 NrOfBotsToSpawn;

	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category = "Gamemode")
	float TimeBetweenWaves;

protected:

	virtual void Tick(float DeltaSeconds) override;

	// Hook for blueprint to spawn a single bot
	UFUNCTION(BlueprintImplementableEvent, Category = "Gamemode")
	void SpawnNewBot();

	void SpawnBotTimerElapsed();

	void StartWave();

	void EndWave();

	void CheckWaveState();

	void CheckPlayersAlive();

	void GameOver();

	// Set Timer for next StartWave
	void WaveIntermission();	

public:

	ASGameMode();

	virtual void StartPlay() override;

};
