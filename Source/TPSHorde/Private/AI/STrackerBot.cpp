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
#include "Sound/SoundCue.h"

static int32 DebugTrackerBotDrawing = 0;
FAutoConsoleVariableRef CVARDebugTrackerBotDrawing(
	TEXT("COOP.DebugTrackerBots"),
	DebugTrackerBotDrawing,
	TEXT("Draw Debug Lines for TrackerBots"),
	ECVF_Cheat);

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
	SelfDamageInterval = 0.5;
	MaxDamageBoost = 4;
	CheckBotRadius = 600;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComp->SetSphereRadius(ExplosionRadius);

	if (Role == ROLE_Authority)
	{
		// Find initial move-to point
		NextPathPoint = GetNextPathPoint();
	}

	GetWorldTimerManager().SetTimer(TimerHandle_SwarmScan, this, &ASTrackerBot::CheckNearbyBots, 1.0f, true, 0);
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority && !bExploded)
	{
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

			if (DebugTrackerBotDrawing > 0)
			{
				DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Yellow, false, 0.0f, 0, 1.f);
			}
		}

		if (DebugTrackerBotDrawing > 0)
		{
			DrawDebugSphere(GetWorld(), NextPathPoint, 20.0f, 32, FColor(255, 0, 0), false, 0.0f, 0, 1.0f);
		}		
	}
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

	if (NavPath && NavPath->PathPoints.Num() > 1)
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

	bExploded = true;

	MeshComp->SetVisibility(false, true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFX, GetActorTransform());
	UGameplayStatics::PlaySoundAtLocation(this, SFX_Explosion, GetActorLocation());

	if (Role == ROLE_Authority)
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);

		float ActualDamage = ExplosionDamage + (ExplosionDamage * DamageBoost);

		UGameplayStatics::ApplyRadialDamage(this, ActualDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

		if (DebugTrackerBotDrawing > 0)
		{
			DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 2.0f, 0, 1.0f);
		}

		SetLifeSpan(1.f);
	}	
}

void ASTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 25, GetInstigatorController(), this, nullptr);
}

// TODO redo this method
// Current issue is that the new spherecomp is being used for the selfdestruct range
void ASTrackerBot::CheckNearbyBots()
{
	//Create temporary collision for overlap
	FCollisionShape CollShape;
	CollShape.SetSphere(CheckBotRadius);

	// QueryParams to only find Pawns (eg. player or other bots)
	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_Pawn);
	QueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

	// Get array of overlapped actors
	TArray<FOverlapResult> OverlapResults;
	GetWorld()->OverlapMultiByObjectType(OverlapResults, GetActorLocation(), FQuat::Identity, QueryParams, CollShape);

	if (DebugTrackerBotDrawing > 0)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), CheckBotRadius, 12, FColor::White, false, 1.0f);
	}

	int32 NrOfBots = 0;

	for (FOverlapResult Result : OverlapResults)
	{
		// cast to check if overlapped actor is a Tracker Bot
		ASTrackerBot* Bot = Cast<ASTrackerBot>(Result.GetActor());

		// ignore self and only count other bots
		if (Bot && Bot != this)
		{
			NrOfBots++;
		}
	}

	DamageBoost = FMath::Clamp(NrOfBots, 0, MaxDamageBoost);

	// Modify tracker bot material based on DamageBoost
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	
	if (MatInst)
	{
		float Alpha = DamageBoost / (float)MaxDamageBoost;

		MatInst->SetScalarParameterValue("DamageBoostAlpha", Alpha);
	}

	if (DebugTrackerBotDrawing > 0)
	{
		DrawDebugString(GetWorld(), GetActorLocation(), *FString::FromInt(DamageBoost), nullptr, FColor::White, 1.0f, true);
	}
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (!bIsSelfDestructing && !bExploded)
	{
		ASCharacter* PlayerCharacter = Cast<ASCharacter>(OtherActor);

		if (PlayerCharacter)
		{
			
			if (Role == ROLE_Authority)
			{
				// if overlapped actor is a player, initiate self destruct function
				GetWorldTimerManager().SetTimer(TimerHandle_SelfDestruct, this, &ASTrackerBot::DamageSelf, SelfDamageInterval, true, 0);
			}
						
			bIsSelfDestructing = true;

			UGameplayStatics::SpawnSoundAttached(SFX_SelfDestruct, RootComponent);
		}
	}
}

