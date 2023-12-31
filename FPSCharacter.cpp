// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "Camera/CameraComponent.h"
#include <Net/UnrealNetwork.h>
#include "FPSWeapon.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = true;
	Camera->SetupAttachment(GetMesh(), FName("head"));


}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		for (const TSubclassOf<AFPSWeapon>& WeaponClass : DefaultWeapons)
		{
			if (!WeaponClass)continue;
			FActorSpawnParameters Params;
			Params.Owner = this;
			AFPSWeapon* SpawnedWeapon = GetWorld()->SpawnActor<AFPSWeapon>(WeaponClass, Params);
			const int32 Index = Weapons.Add(SpawnedWeapon);
			if (Index == CurrentIndex)
			{
				CurrentWeapon = SpawnedWeapon;
				OnRep_CurrentWeapon(nullptr);
			}
		}
	}
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AFPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &AFPSCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName("LookRight"), this, &AFPSCharacter::LookRight);

}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AFPSCharacter, Weapons, COND_None);
	DOREPLIFETIME_CONDITION(AFPSCharacter, CurrentWeapon, COND_None);
}

void AFPSCharacter::OnRep_CurrentWeapon(const AFPSWeapon* OldWeapon)
{
	if (CurrentWeapon) 
	{
		if (CurrentWeapon->CurrentOwner)
		{
			CurrentWeapon->SetActorTransform(GetMesh()->GetSocketTransform(FName("weaponsocket_r")), false, nullptr, ETeleportType::TeleportPhysics);
			CurrentWeapon->AttachToComponent(GetMesh(),FAttachmentTransformRules::KeepWorldTransform, FName("weaponsocket_r"));

			CurrentWeapon->Mesh->SetVisibility(true);
			CurrentWeapon->CurrentOwner = this;
		}
	}

	if (OldWeapon) 
	{

	}
}

void AFPSCharacter::MoveForward(const float Value)
{
	
	AddMovementInput(GetActorForwardVector() * Value);
}

void AFPSCharacter::MoveRight(const float Value)
{
	
	AddMovementInput(GetActorRightVector() * Value);
}

void AFPSCharacter::LookUp(const float Value)
{
	AddControllerPitchInput(Value);
}

void AFPSCharacter::LookRight(const float Value)
{
	AddControllerYawInput(Value);
}

