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
	
}

void ASGrenadeLauncher::StartFire()
{
	Fire();
}

void ASGrenadeLauncher::Fire()
{
	if (GrenadeProjectile)
	{
		FVector EyesLocation = FVector(0);
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FRotator MuzzleRotation;

		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			ASCharacter* CharacterPawn = Cast<ASCharacter>(MyOwner);
			if (CharacterPawn)
			{
				CharacterPawn->GetActorEyesViewPoint(EyesLocation, MuzzleRotation);
			}	
		}

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		ActorSpawnParams.Instigator = nullptr;

		GetWorld()->SpawnActor<ASGrenadeProjectile>(GrenadeProjectile, MuzzleLocation, MuzzleRotation, ActorSpawnParams);
	}
}
