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
#include "Net/UnrealNetwork.h"


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
	BulletSpread = 2.0f;
	FireRate = 450.0f;
	MaxAmmo = 60;

	SetReplicates(true);

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	CurrentAmmo = MaxAmmo;

	TimeBetweenShots = 60 / FireRate;
}

void ASWeapon::Fire()
{
	if (Role < ROLE_Authority)
	{
		// if less than Authority send a request to server host to execute fire
		ServerFire();
	}

	//Trace a line in worldspace from pawn camera to crosshair location
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		ASCharacter* PlayerCharacter = Cast<ASCharacter>(MyOwner);

		FVector CameraLocation;
		FRotator CameraRotation;
		MyOwner->GetActorEyesViewPoint(CameraLocation, CameraRotation);

		FVector ShotDirection = CameraRotation.Vector();

		float HalfRad = FMath::DegreesToRadians(BulletSpread);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

		FVector TraceEnd = CameraLocation + (ShotDirection * 10000);

		// structure for parameters to be checked by linetrace
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		// set the end default "Target" for tracer effect be line trace end
		TracerEndPoint = TraceEnd;
		
		EPhysicalSurface SurfaceType = SurfaceType_Default;

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
			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float WeaponDamage = BaseWeaponDamage;
			if (SurfaceType == SURFACE_FLESHHEAD)
			{
				WeaponDamage *= HeadshotDamageMultiplier;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, WeaponDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);
			
			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

			TracerEndPoint = Hit.ImpactPoint;
		}

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), CameraLocation, TraceEnd, FColor::Green, false, 1.0f, 0, 1.0f);
		}

		PlayFireEffects(TracerEndPoint);

		if (CurrentAmmo > 0)
		{
			//TODO change to simply decrement operator
			CurrentAmmo -= 1;
		}
		else
		{
			// stop firing if we have no more ammo
			EndFire();
		}

		// If we're on the host or server then we replicate these variables when they change
		if (Role == ROLE_Authority)
		{
			HitScanTrace.TraceTo = TracerEndPoint;
			HitScanTrace.SurfaceType = SurfaceType;
		}

		LastFireTime = GetWorld()->TimeSeconds;
	}
	else
	{
		EndFire();
	}
}

void ASWeapon::OnRep_HitScanTrace()
{
	// Play cosmetic FX
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ASWeapon::ServerFire_Implementation()
{
	// This function is running on the host
	Fire();
}

bool ASWeapon::ServerFire_Validate()
{
	//usually used as anti-cheat
	return true;
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

void ASWeapon::Reload(int AmmoReserve)
{
	int ReloadCount = MaxAmmo - CurrentAmmo;

	if (AmmoReserve >= ReloadCount)
	{
		CurrentAmmo += ReloadCount;
	}
}

void ASWeapon::PlayFireEffects(FVector TraceEnd)
{
	FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, MuzzleLocation, 0.2f);
	}

	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter("Target", TraceEnd);
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

void ASWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
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
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

void ASWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
}