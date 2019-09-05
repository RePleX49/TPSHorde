// Fill out your copyright notice in the Description page of Project Settings.

#include "STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "SHealthComponent.h"
#include "SCharacter.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(200); //set radius here in case not defined later
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetupAttachment(RootComponent);

	MoveForce = 1000;
	bUseVelocityChange = false;
	TargetDistanceThreshold = 100;
	ExplosionDamage = 40;
	ExplosionRadius = 200;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComp->SetSphereRadius(ExplosionRadius);

	// Find initial move-to point
	NextPathPoint = GetNextPathPoint();
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	// Checking if MatInst is actually created in case a material isn't assigned
	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health), *GetName());
    
	// Explode when HP reaches 0
	if (Health <= 0)
	{
		SelfDestruct();
	}
}

FVector ASTrackerBot::GetNextPathPoint()
{
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	if (NavPath->PathPoints.Num() > 1)
	{
		// return the next path point
		return NavPath->PathPoints[1];
	}

	// Failed to find path
	return GetActorLocation();
}

void ASTrackerBot::SelfDestruct()
{
	// Check if already exploded in case called twice in same frame
	if (bExploded)
	{
		return;
	}

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFX, GetActorTransform());

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 2.0f, 0, 1.0f);

	bExploded = true;

	Destroy();
}

void ASTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 25, GetInstigatorController(), this, nullptr);
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

	if (DistanceToTarget <= TargetDistanceThreshold)
	{
		NextPathPoint = GetNextPathPoint();	
	}
	else
	{
		// Move to next target
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();

		ForceDirection *= MoveForce;

		MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Yellow, false, 0.0f, 0, 1.f);
	}

	DrawDebugSphere(GetWorld(), NextPathPoint, 20.0f, 32, FColor(255, 0, 0), false, 0.0f, 0, 1.0f);
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	ASCharacter* PlayerCharacter = Cast<ASCharacter>(OtherActor);

	if (PlayerCharacter)
	{
		if (!bIsSelfDestructing)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_SelfDestruct, this, &ASTrackerBot::DamageSelf, 1.0f, true, 0);
			bIsSelfDestructing = true;
		}
	}
}

