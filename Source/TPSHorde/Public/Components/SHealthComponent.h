// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"

//OnHealthChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, USHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(COOP), meta=(BlueprintSpawnableComponent) )
class TPSHORDE_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHealthComponent();

	// TODO: add Replicated if planning to implement team switching
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
	uint8 TeamNum;

	float GetHealth() const;

protected:

	virtual void BeginPlay() override;

	bool bIsDead;

	UPROPERTY(ReplicatedUsing=OnRep_HealthChanged, EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
	float HealthPoints;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthComponent")
	float MaxHealth;

	// Marked UFUNCTION since being used as events/delegate
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, 
		AActor* DamageCauser);

	UFUNCTION()
	void OnRep_HealthChanged();

	float ReceivedDamage;

public:

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HealthComponents")
	static bool IsFriendly(AActor* ActorA, AActor* ActorB);

	UFUNCTION(BlueprintCallable, Category = "HealthComponents")
	void Heal(float HealPerTick);
};
