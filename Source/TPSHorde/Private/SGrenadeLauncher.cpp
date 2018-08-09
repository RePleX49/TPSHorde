// Fill out your copyright notice in the Description page of Project Settings.

#include "SGrenadeLauncher.h"
#include "SGrenadeProjectile.h"
#include "Components/SkeletalMeshComponent.h"
#include "SCharacter.h"
#include "Engine/World.h"

ASGrenadeLauncher::ASGrenadeLauncher()
{
	// constructor
	FName MuzzleSocketName = "MuzzleSocket";
}

void ASGrenadeLauncher::BeginPlay()
{
	PlayerPawn = Cast<ASCharacter>(GetOwner());
}

void ASGrenadeLauncher::Fire()
{
	if (GrenadeProjectile)
	{
		FVector EyesLocation = FVector(0);
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FRotator MuzzleRotation;

		if (PlayerPawn)
		{
			PlayerPawn->GetActorEyesViewPoint(EyesLocation, MuzzleRotation);
		}	

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
