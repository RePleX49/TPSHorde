// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplodingBarrel.generated.h"

class USHealthComponent;
class USphereComponent;

UCLASS()
class TPSHORDE_API ASExplodingBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASExplodingBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* ExplosionFX;

	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	UMaterial* ExplodedMaterial;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	bool bExploded;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	TArray<AActor*> IgnoredActors;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float ExplosionForce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	TSubclassOf<UDamageType> BarrelDamageType;

	
	
};
