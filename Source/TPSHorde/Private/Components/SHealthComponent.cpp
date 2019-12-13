// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	MaxHealth = 100.0f;
	HealthPoints = MaxHealth;

	SetIsReplicated(true);
}

//Added const to suggest read only
float USHealthComponent::GetHealth() const
{
	return HealthPoints;
}

void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	//Only hook if we are server
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}
	}	
}

void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, 
	AActor* DamageCauser)
{
	ReceivedDamage = Damage;
	if (Damage <= 0.0f)
	{
		return;
	}

	// Update health clamped
	HealthPoints = FMath::Clamp(HealthPoints - Damage, 0.0f, MaxHealth);

	UE_LOG(LogTemp, Log, TEXT("Current Health: %s"), *FString::SanitizeFloat(HealthPoints));

	// this is the event delegate broadcast
	OnHealthChanged.Broadcast(this, HealthPoints, Damage, DamageType, InstigatedBy, DamageCauser);
}

void USHealthComponent::OnRep_HealthChanged()
{
	OnHealthChanged.Broadcast(this, HealthPoints, ReceivedDamage, nullptr, nullptr, nullptr);
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, HealthPoints);
}
