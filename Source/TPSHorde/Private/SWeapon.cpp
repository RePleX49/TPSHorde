// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include <DrawDebugHelpers.h>
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASWeapon::Fire()
{
	//Trace a line in worldspace from pawn camera to crosshair location
	AActor* MyOwner = GetOwner();
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

		// set the end default "Target" for tracer effect be line trace end
		FVector TracerEnd = TraceEnd;
		
		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, CameraLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			// Blocking Hit, process damage
			AActor* HitActor = Hit.GetActor();
			if (HitActor)
			{
				UGameplayStatics::ApplyPointDamage(HitActor, 10.f, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
			}

			if (ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			TracerEnd = Hit.ImpactPoint;
		}

		// DrawDebugLine(GetWorld(), CameraLocation, TraceEnd, FColor::Green, false, 1.0f, 0, 1.0f);

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
		
	}
	
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

