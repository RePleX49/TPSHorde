// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UParticleSystem;

UCLASS()
class TPSHORDE_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

	void StartFire();

	void EndFire();

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USkeletalMeshComponent* MeshComp;

	void PlayFireEffects();

	virtual void Fire();

	FTimerHandle TimerHandle_TimeBetweenShots;

	float LastFireTime;

	// Fire rate in RPM
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float FireRate;

	float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* TracerEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	FVector TracerEnd;

	// Creates a Subclass of CameraShake that we will assign in blueprint
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> CamShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseWeaponDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float HeadshotDamageMultiplier;
public:	

	
	
};
