// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SHomingRocketLauncher.generated.h"

class ASHomingRocket;
class ASCharacter;

UCLASS()
class TPSHORDE_API ASHomingRocketLauncher : public ASWeapon
{
	GENERATED_BODY()
	
public:
	ASHomingRocketLauncher();

	AActor* LockedOnActor;

protected:
	void Tick(float DeltaTime) override;

	void BeginPlay() override;
	
	void StartFire() override;

	void Fire() override;

	void GetTarget();

	ASCharacter* PlayerPawn;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ASHomingRocket> HomingRocket;

	FTimerHandle TimerHandle_GetTarget;
};
