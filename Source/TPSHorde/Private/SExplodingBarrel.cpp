// Fill out your copyright notice in the Description page of Project Settings.

#include "SExplodingBarrel.h"
#include "SHealthComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASExplodingBarrel::ASExplodingBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Comp"));
	RootComponent = MeshComp;

	MeshComp->SetSimulatePhysics(true);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Damage Collider"));
	SphereComp->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale);
	SphereComp->SetSphereRadius(200.0f);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("Health Comp"));

	HealthComp->OnHealthChanged.AddDynamic(this, &ASExplodingBarrel::OnHealthChanged);

	bExploded = false;
	ExplosionForce = 100.0f;
	ExplosionDamage = 50.0f;
}

// Called when the game starts or when spawned
void ASExplodingBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASExplodingBarrel::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, 
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0 && !bExploded)
	{
		bExploded = true;
		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), SphereComp->GetScaledSphereRadius(), BarrelDamageType, IgnoredActors);
		MeshComp->AddImpulse(GetActorUpVector() * ExplosionForce, NAME_None, true);
		MeshComp->SetMaterial(0, ExplodedMaterial);
		if (ExplosionFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFX, GetActorTransform());
		}	

		SetLifeSpan(30.0f);
	}
}

