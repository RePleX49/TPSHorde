// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include "SCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include <DrawDebugHelpers.h>
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TPSHorde.h"
#include "TimerManager.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"), 
	DebugWeaponDrawing, 
	TEXT("Draw Debug Lines for Weapons"), 
	ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	HeadshotDamageMultiplier = 3.0f;
	BaseWeaponDamage = 20.0f;
	FireRate = 450.0f;
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / FireRate;
}

void ASWeapon::Fire()
{
	//Trace a line in worldspace from pawn camera to crosshair location
	AActor* MyOwner = GetOwner();
	ASCharacter* PlayerCharacter = Cast<ASCharacter>(MyOwner);
	if (MyOwner)
	{
		FVector CameraLocation;
		FRotator CameraRotation;
		MyOwner->GetActorEyesViewPoint(CameraLocation, CameraRotation);

		FVector ShotDirection = CameraRotation.Vector();

		FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * 10000);

		// structure for parameters to be checked by linetrace
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		// set the end default "Target" for tracer effect be line trace end
		TracerEnd = TraceEnd;
		
		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, CameraLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			// Blocking Hit, process damage
			AActor* HitActor = Hit.GetActor();

			/*
			PhysMaterial is a Weak Object pointer which means that it is allowed to be
			removed from memory when not needed

			Weak object pointer use dot operators
			*/
			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float WeaponDamage = BaseWeaponDamage;
			if (SurfaceType == SURFACE_FLESHHEAD)
			{
				WeaponDamage *= HeadshotDamageMultiplier;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, WeaponDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
			 
			// TODO make switch statement a function with out parameter for SelectedEffect
			UParticleSystem* SelectedEffect = nullptr;
			switch (SurfaceType)
			{
			case SURFACE_FLESHDEFAULT:
				SelectedEffect = FleshImpactEffect;
				break;	
			case SURFACE_FLESHHEAD:
				SelectedEffect = FleshImpactEffect;
				break;
			default:
				SelectedEffect = DefaultImpactEffect;
				break;
			}

			if (SelectedEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			TracerEnd = Hit.ImpactPoint;
		}

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), CameraLocation, TraceEnd, FColor::Green, false, 1.0f, 0, 1.0f);
		}

		PlayFireEffects();
		if (PlayerCharacter->PrimaryCurrentMagCount > 0)
		{
			PlayerCharacter->PrimaryCurrentMagCount -= 1;
		}
		else
		{
			PlayerCharacter->EndFire();
		}

		LastFireTime = GetWorld()->TimeSeconds;
	}
}

void ASWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ASWeapon::EndFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::PlayFireEffects()
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

	if (TracerEffect)
	{
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter("Target", TracerEnd);
		}
	}

	// Cast to make sure we have APawn owner
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		// Cast to access the PlayerController
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(CamShake);
		}
	}
}