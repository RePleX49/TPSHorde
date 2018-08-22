// Fill out your copyright notice in the Description page of Project Settings.

#include "SHomingRocketLauncher.h"
#include "SCharacter.h"
#include "SHomingRocket.h"
#include "Engine/World.h"


ASHomingRocketLauncher::ASHomingRocketLauncher()
{
	FName MuzzleSocketName = "MuzzleSocket";
}

void ASHomingRocketLauncher::BeginPlay()
{
	PlayerPawn = Cast<ASCharacter>(GetOwner());
}

void ASHomingRocketLauncher::Fire()
{
	if (HomingRocket)
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

		GetWorld()->SpawnActor<ASHomingRocket>(HomingRocket, MuzzleLocation, MuzzleRotation, ActorSpawnParams);
	}
}
