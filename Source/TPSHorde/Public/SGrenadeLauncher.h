// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SGrenadeLauncher.generated.h"

class ASGrenadeProjectile;
class USkeletalMeshComponent;

/**
 * 
 */
UCLASS()
class TPSHORDE_API ASGrenadeLauncher : public ASWeapon
{
	GENERATED_BODY()
	
public:
	//Constructor method
	ASGrenadeLauncher();

protected:
	void Tick(float DeltaTime) override;

	void Fire() override;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ASGrenadeProjectile> GrenadeProjectile;
};
