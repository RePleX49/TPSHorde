// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthComponent.h"


// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	MaxHealth = 100.0f;
	HealthPoints = MaxHealth;
}

void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
	}
}

void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, 
	AActor* DamageCauser)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	// Update health clamped
	HealthPoints = FMath::Clamp(HealthPoints - Damage, 0.0f, MaxHealth);

	UE_LOG(LogTemp, Log, TEXT("Current Health: %s"), *FString::SanitizeFloat(HealthPoints));
}

