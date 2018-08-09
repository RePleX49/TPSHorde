// Fill out your copyright notice in the Description page of Project Settings.

#include "SGrenadeProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Particles/ParticleSystem.h"

// Sets default values
ASGrenadeProjectile::ASGrenadeProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = 1000.0f;
	ProjectileMovementComp->MaxSpeed = 1000.0f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->bShouldBounce = true;
}

// Called when the game starts or when spawned
void ASGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &ASGrenadeProjectile::Explode, 1.0f, false);
}

void ASGrenadeProjectile::Explode()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	UGameplayStatics::ApplyRadialDamage(this, 60.0f, GetActorLocation(), 100.0f, DamageType, IgnoreActors);

	GetWorld()->GetTimerManager().ClearTimer(UnusedHandle);

	Destroy();
}

