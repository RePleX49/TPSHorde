// Fill out your copyright notice in the Description page of Project Settings.

#include "SHomingRocketLauncher.h"
#include "SCharacter.h"
#include "SHomingRocket.h"
#include "TimerManager.h"
#include "Engine/World.h"


ASHomingRocketLauncher::ASHomingRocketLauncher()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	FName MuzzleSocketName = "MuzzleSocket";
}

void ASHomingRocketLauncher::BeginPlay()
{
	PlayerPawn = Cast<ASCharacter>(GetOwner());
}

void ASHomingRocketLauncher::Tick(float DeltaTime)
{
	UE_LOG(LogTemp, Log, TEXT("Ticking"));
	if (PlayerPawn)
	{
		UE_LOG(LogTemp, Log, TEXT("Got parent: %s"), *PlayerPawn->GetName());
		if (PlayerPawn->bIsAiming)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_GetTarget, this, &ASHomingRocketLauncher::GetTarget, 0.25f, true);
			UE_LOG(LogTemp, Log, TEXT("SetGetTargetTimer"));
		}
		else
		{
			GetWorldTimerManager().ClearTimer(TimerHandle_GetTarget);
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Did not get parent"));
	}
}

void ASHomingRocketLauncher::StartFire()
{
	Fire();
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

		UE_LOG(LogTemp, Log, TEXT("Grenade Launcher Fired"));

		ASHomingRocket* Rocket = GetWorld()->SpawnActor<ASHomingRocket>(HomingRocket, MuzzleLocation, MuzzleRotation, ActorSpawnParams);
		Rocket->SetTargetActor(LockedOnActor);
	}
}

void ASHomingRocketLauncher::GetTarget()
{
	FHitResult Hit;
	FVector StartVector = GetActorLocation();
	FVector EndVector = StartVector + (GetActorForwardVector() * 10000);
	FCollisionQueryParams QueryParams;
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartVector, EndVector, ECC_Visibility, QueryParams))
	{
		LockedOnActor = Hit.GetActor();
		UE_LOG(LogTemp, Log, TEXT("Locked On To %s"), *LockedOnActor->GetName());
	}
}