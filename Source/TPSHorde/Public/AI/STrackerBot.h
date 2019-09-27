// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

class USHealthComponent;
class USphereComponent;
class USoundCue;

UCLASS()
class TPSHORDE_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USHealthComponent* HealthComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* SphereComp;

	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);

	FVector GetNextPathPoint();

	//Next point in navigation path
	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float MoveForce;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float TargetDistanceThreshold;

	// Dynamic Material to pulse when taking damage
	UMaterialInstanceDynamic* MatInst;

	void SelfDestruct();

	void DamageSelf();

	void CheckNearbyBots();

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	UParticleSystem* ExplosionFX;

	int DamageBoost;

	int MaxDamageBoost;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float CheckBotRadius;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float ExplosionRadius;

	// bool to make sure we don't run any code after we called SelfDestruct
	bool bExploded;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float SelfDamageInterval;

	bool bIsSelfDestructing;

	FTimerHandle TimerHandle_SelfDestruct;
	FTimerHandle TimerHandle_SwarmScan;

	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundCue* SFX_SelfDestruct;

	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundCue* SFX_Explosion;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
