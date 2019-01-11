// Fill out your copyright notice in the Description page of Project Settings.

#include "SHomingRocket.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
ASHomingRocket::ASHomingRocket()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

}

// Called when the game starts or when spawned
void ASHomingRocket::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASHomingRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TargetActor)
	{
		UE_LOG(LogTemp, Log, TEXT("Locked on target is %s"), *TargetActor->GetName());
		FVector DirectionVector = TargetActor->GetActorLocation() - GetActorLocation();
		MeshComp->AddForce(DirectionVector * 500.0f, NAME_None, false);
	}
}

void ASHomingRocket::SetTargetActor(AActor* LockedTarget)
{
	TargetActor = LockedTarget;
}


