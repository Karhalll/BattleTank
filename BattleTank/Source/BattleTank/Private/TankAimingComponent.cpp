// Copyright FukasD Inc.

#include "TankAimingComponent.h"
#include "TankBarrel.h"
#include "TankTurret.h"
#include "Projectile.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"

// Sets default values for this component's properties
UTankAimingComponent::UTankAimingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true; 

	// ...
}

void UTankAimingComponent::BeginPlay()
{
	// So that first fire is after initial reload
	LastFireTime = GetWorld()->GetTimeSeconds();
}

void UTankAimingComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (bool isReloaded = (GetWorld()->GetTimeSeconds() - LastFireTime) > ReloadTimeInSeconds)
	{
		FiringStatus = EFiringStatus::Reloading;
	}
	// TODO handle aiming and locked states
}

void UTankAimingComponent::Initialise(UTankBarrel* BarelToSet, UTankTurret* TurretToSet)
{
	Barrel = BarelToSet;
	Turret = TurretToSet;

}

void UTankAimingComponent::AimAt(FVector HitLocation)
{
	if (!ensure(Barrel)) { return; }

	FVector OutLaunchVelocity;
	FVector StartLocation = Barrel->GetSocketLocation(FName("Projectile"));
	bool bHaveAimSolution = UGameplayStatics::SuggestProjectileVelocity
	(
		this,
		OutLaunchVelocity,
		StartLocation,
		HitLocation,
		LaunchSpeed,
		false,
		0,
		0,
		ESuggestProjVelocityTraceOption::DoNotTrace
	);
	if (bHaveAimSolution)
	{
		auto AimDirection = OutLaunchVelocity.GetSafeNormal();
		MoveBarrelTowards(AimDirection);
		MoveTurretTowards(AimDirection);
	}
}

void UTankAimingComponent::MoveBarrelTowards(FVector AimDirection)
{
	if (!ensure(Barrel) || !ensure(Turret)) { return; }
	// Work-out difference between current barrel rotation and AimDirection
	auto BarrelRotation = Barrel->GetForwardVector().Rotation();
	auto AimAsRotator = AimDirection.Rotation();
	auto DeltaRotator = AimAsRotator - BarrelRotation;
	
	Barrel->Elevate(DeltaRotator.Pitch);
}

void UTankAimingComponent::MoveTurretTowards(FVector AimDirection)
{
	if (!ensure(Barrel) || !ensure(Turret)) { return; }
	// Work-out difference between current barrel rotation and AimDirection
	float TurretRotation = To360(Turret->GetForwardVector().Rotation().Yaw);
	float AimAsRotator = To360(AimDirection.Rotation().Yaw);
	
	float DeltaRotator = AimAsRotator - TurretRotation;

	if (DeltaRotator > 180.f)
	{
		DeltaRotator -= 360.f;
	}

	if (((270.f < TurretRotation) && (TurretRotation < 360.f)) && ((0.f < AimAsRotator) &&  (AimAsRotator < 90.f)))
	{
		DeltaRotator = -DeltaRotator;
	}

	Turret->Rotate(DeltaRotator);
}

float UTankAimingComponent::To360(float Number)
{
	float Number2 = Number;
	if(Number2 < 0)
	{
		Number2 += 360.f;
		return Number2;
	}
	return Number2;
}

void UTankAimingComponent::Fire()
{
	if (FiringStatus != EFiringStatus::Reloading)
	{
		// Spawn a projectile at the socket location
		if (!ensure(Barrel)) { return; }
		if (!ensure(ProjectileBlueprint)) { return; }
		auto Projectile = GetWorld()->SpawnActor<AProjectile>(
			ProjectileBlueprint,
			Barrel->GetSocketLocation(FName("Projectile")),
			Barrel->GetSocketRotation(FName("Projectile"))
			);

		Projectile->LaunchProjectile(LaunchSpeed);
		LastFireTime = GetWorld()->GetTimeSeconds();
	}
}