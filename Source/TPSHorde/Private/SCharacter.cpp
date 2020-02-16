// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "SWeapon.h"
#include "SHealthComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "TPSHorde.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanJump = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	AimedFOV = 60.0f;
	AimInterpSpeed = 10.0f;

	bIsCrouching = false;
	bIsFiring = false;
	bIsReloading = false;
	bIsDead = false;

	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	AimWalkSpeed = 100.0f;
	RunSpeed = 400.0f;

	AmmoReserves = 180;

	ReloadSpeed = 2.1f;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;
	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	if (Role == ROLE_Authority)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "WeaponSocket");
		}
	}

	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ASCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ASCharacter::ToggleCrouch()
{
	if (bIsCrouching)
	{
		UnCrouch();
		bIsCrouching = false;
	}
	else
	{
		Crouch();
		bIsCrouching = true;
	}
}

void ASCharacter::DoJump()
{
	Jump();
}

void ASCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
		bIsFiring = true;
	}
}

void ASCharacter::EndFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->EndFire();
		bIsFiring = false;
	}
}

void ASCharacter::BeginAim()
{
	if (!bIsAiming)
	{
		bIsAiming = true;
		GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;
	}
}

void ASCharacter::EndAim()
{
	if (bIsAiming)
	{
		bIsAiming = false;
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	}
}

void ASCharacter::StartReload()
{
	bIsReloading = true;
	GetWorldTimerManager().SetTimer(TimerHandle_Reload, this, &ASCharacter::Reload, ReloadSpeed, false);
}

void ASCharacter::Reload()
{
	CurrentWeapon->Reload(AmmoReserves);
	GetWorldTimerManager().ClearTimer(TimerHandle_Reload);
	bIsReloading = false;
}

void ASCharacter::EquipPrimary()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (CurrentWeapon)
	{
		GetWorld()->DestroyActor(CurrentWeapon);
		
		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(EquippedWeapons[0], FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "WeaponSocket");
		}
	}	
}

void ASCharacter::EquipSecondary()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (CurrentWeapon)
	{
		GetWorld()->DestroyActor(CurrentWeapon);
		
		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(EquippedWeapons[1], FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "WeaponSocket");
		}
	}	
}

void ASCharacter::StartRun()
{
	if (!bIsAiming && !bIsCrouching && !bIsFiring)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
}

void ASCharacter::EndRun()
{
	if (!bIsAiming && !bIsCrouching && !bIsFiring)
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	}
}

void ASCharacter::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, 
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0 && !bIsDead)
	{
		//Die
		bIsDead = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DetachFromControllerPendingDestroy();
		EndFire();

		SetLifeSpan(10.0f);
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// if bIsAiming is true we set to AimedFOV, otherwise set to DefaultFOV
	float TargetFOV = bIsAiming ? AimedFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, AimInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::ToggleCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::DoJump);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ASCharacter::StartRun);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ASCharacter::EndRun);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ASCharacter::BeginAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ASCharacter::EndAim);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::EndFire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASCharacter::StartReload);
	PlayerInputComponent->BindAction("PrimarySelect", IE_Pressed, this, &ASCharacter::EquipPrimary);
	PlayerInputComponent->BindAction("SecondarySelect", IE_Pressed, this, &ASCharacter::EquipSecondary);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
}
	
void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, bIsDead);
}
