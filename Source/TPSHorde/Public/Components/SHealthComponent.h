// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"


UCLASS( ClassGroup=(COOP), meta=(BlueprintSpawnableComponent) )
class TPSHORDE_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHealthComponent();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
	float HealthPoints;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthComponent")
	float MaxHealth;

	// Marked UFUNCTION since being used as events/delegate
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
};
