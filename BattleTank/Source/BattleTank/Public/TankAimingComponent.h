// Copyright FukasD Inc.

#pragma once

#include "CoreMinimal.h"
#include "TankAimingComponent.generated.h"

// Enum for aiming state
UENUM()
enum class EFiringStatus : uint8
{
	Aiming,
	Reloading,
	Locked
};

// Forward Declaration
class UTankBarrel; 
class UTankTurret;
class AProjectile;

// Holds barrel's properties and Elevate method
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BATTLETANK_API UTankAimingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void Initialise(UTankBarrel* BarelToSet, UTankTurret* TurretToSet);

	void AimAt(FVector HitLocation);

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void Fire();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "State")
	EFiringStatus FiringStatus = EFiringStatus::Reloading;

private:
	// Sets default values for this component's properties
	UTankAimingComponent();

	bool IsBarrelMoving();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	void MoveBarrelTowards(FVector AimDirection);
	void MoveTurretTowards(FVector AimDirection);

	UTankBarrel* Barrel = nullptr;
	UTankTurret* Turret = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	float LaunchSpeed = 10000;

	

	float To360(float Number);

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
		float ReloadTimeInSeconds = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
		TSubclassOf<AProjectile> ProjectileBlueprint;

	double LastFireTime = 0;
	FVector AimDirection;
};
