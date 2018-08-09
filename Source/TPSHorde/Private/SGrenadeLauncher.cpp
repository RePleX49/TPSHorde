// Fill out your copyright notice in the Description page of Project Settings.

#include "SGrenadeLauncher.h"
#include "SGrenadeProjectile.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

ASGrenadeLauncher::ASGrenadeLauncher()
{
	// constructor
}

void ASGrenadeLauncher::Fire()
{
	if (GrenadeProjectile)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation("MuzzleSocket");
		FRotator MuzzleRotation = MeshComp->GetSocketRotation("MuzzleSocket");

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		ActorSpawnParams.Instigator = nullptr;

		GetWorld()->SpawnActor<ASGrenadeProjectile>(GrenadeProjectile, MuzzleLocation, MuzzleRotation, ActorSpawnParams);
	}
}

// Called every frame
void ASGrenadeLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
