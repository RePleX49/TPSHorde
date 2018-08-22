// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SHomingRocketLauncher.generated.h"

class ASHomingRocket;
class ASCharacter;

/**
 * 
 */
UCLASS()
class TPSHORDE_API ASHomingRocketLauncher : public ASWeapon
{
	GENERATED_BODY()
	
public:
	ASHomingRocketLauncher();

protected:

	void BeginPlay() override;
	
	void Fire() override;

	ASCharacter* PlayerPawn;

	TSubclassOf<ASHomingRocket> HomingRocket;
};
