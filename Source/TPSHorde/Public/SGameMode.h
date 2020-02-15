// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

enum class EWaveState : uint8;

// send KILLED actor, KILLER actor, and KILLER controller
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController); 

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


	UPROPERTY(EditDefaultsOnly, Category = "Gamemode")
	float TimeBetweenWaves;

protected:

	virtual void Tick(float DeltaSeconds) override;

	// Hook for blueprint to spawn a single bot
	UFUNCTION(BlueprintImplementableEvent, Category = "Gamemode")
	void SpawnNewBot();

	void SpawnBotTimerElapsed();

	// Set Timer for next StartWave
	void WaveIntermission();

	void StartWave();

	void EndWave();

	void CheckWaveState();

	void CheckPlayersAlive();

	void GameOver();

	void SetWaveState(EWaveState NewState);

	void RespawnDeadPlayers();

public:

	ASGameMode();

	virtual void StartPlay() override;

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;

	UPROPERTY(BlueprintReadOnly, Category = "GameMode")
	int32 WaveCount;
};
