// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASWeapon;
class USHealthComponent;

UCLASS()
class TPSHORDE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
	int PrimaryMaxMagCount;

	UPROPERTY(BlueprintReadOnly, Category = "Weapons")
	int PrimaryCurrentMagCount;

	void StartFire();
	void EndFire();

	UPROPERTY(BlueprintReadOnly, Category = "Weapons")
	bool bIsAiming;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void ToggleCrouch();
	bool bIsCrouching;

	void DoJump();

	void BeginAim();
	void EndAim();

	void StartReload();
	void Reload();

	void EquipPrimary();
	void EquipSecondary();

	UPROPERTY(Replicated)
	ASWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<ASWeapon> StarterWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TArray<TSubclassOf<ASWeapon>> EquippedWeapons;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComp;

	UPROPERTY(BlueprintReadOnly, Category = "Weapons")
	bool bIsFiring;

	UPROPERTY(BlueprintReadOnly, Category = "Weapons")
	bool bIsReloading;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float AimedFOV;

	float DefaultFOV;

	// meta constrains the AimInterpSpeed value to be between 0.1 and 100
	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float AimInterpSpeed;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	float DefaultWalkSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float AimWalkSpeed;

	// TODO move reload speed variable to weapon class
	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	float ReloadSpeed;
	
	FTimerHandle TimerHandle_Reload;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, 
		class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Status")
	bool bIsDead;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;
	
};
