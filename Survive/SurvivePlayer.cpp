// Fill out your copyright notice in the Description page of Project Settings.

#include "SurvivePlayer.h"
#include "SurviveProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "BasicRifle.h"
#include "Weapons/SurviveWeaponBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

// Sets default values
ASurvivePlayer::ASurvivePlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	//m_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	//RootComponent = m_RootComponent;

	// Set size for collision capsule
	UCapsuleComponent* capsule = GetCapsuleComponent();
	capsule->InitCapsuleSize(55.f, 96.0f);
	//capsule->SetupAttachment(m_RootComponent);

	// Create a FirstPerson CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(capsule);
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a SpringArmComponent for the ThirdPersonCamera
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(capsule);
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetRelativeLocation(FVector(0.f, 0.f, 45.f));

	// Create Third Person CameraComponent
	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCameraComponent->SetupAttachment(SpringArmComponent);
	ThirdPersonCameraComponent->SetRelativeLocation(FVector(0.f, 45.f, 0.f));
	ThirdPersonCameraComponent->SetActive(false);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	MeshThirdPerson = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMeshThirdPerson"));
	MeshThirdPerson->SetupAttachment(GetCapsuleComponent());
	MeshThirdPerson->bCastDynamicShadow = false;
	MeshThirdPerson->CastShadow = false;
	MeshThirdPerson->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	MeshThirdPerson->SetRelativeLocation(FVector(0.f, 0.f, -97.f));
	if (HasAuthority()) {
		MeshThirdPerson->SetHiddenInGame(true);
		MeshThirdPerson->SetVisibility(false);
	}

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	// TODO: Setup Muzzle location and attachment
	FP_MuzzleLocation->SetupAttachment(capsule);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);
}

// Called when the game starts or when spawned
void ASurvivePlayer::BeginPlay()
{
	Super::BeginPlay();

	// Try and spawn starting weapon
	if (WeaponClass)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			const FRotator SpawnRotation = GetControlRotation();
			const FVector SpawnLocation = GetActorLocation();

			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			//const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the starting weapon
			Weapon = World->SpawnActor<ASurviveWeaponBase>(WeaponClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			Weapon->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
			Weapon->SetOwner(this);

			SecondaryWeapon = World->SpawnActor<ASurviveWeaponBase>(WeaponClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			SecondaryWeapon->AttachToComponent(MeshThirdPerson, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("ShoulderHolster"));
			SecondaryWeapon->SetOwner(this);
			if (HasAuthority()) {
				// Implement hide method in weapon base
				SecondaryWeapon->SetHideMesh(true);
			}
		}
	}
}

// Called every frame
void ASurvivePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASurvivePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASurvivePlayer::OnFire);

	// Toggle First- and ThirdPerson Camera
	PlayerInputComponent->BindAction("ToggleCamera", IE_Pressed, this, &ASurvivePlayer::OnToggleCamera);

	//PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ASurvivePlayer::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ASurvivePlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASurvivePlayer::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASurvivePlayer::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASurvivePlayer::LookUpAtRate);
}

FVector ASurvivePlayer::GetPawnViewLocation() const
{
	if (FirstPersonCameraComponent->IsActive())
		return FirstPersonCameraComponent->GetComponentLocation();
	else if(ThirdPersonCameraComponent->IsActive())
		return ThirdPersonCameraComponent->GetComponentLocation();

	return Super::GetPawnViewLocation();
}

void ASurvivePlayer::OnFire()
{
	if (!Weapon)
		return;

	Weapon->Fire();

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void ASurvivePlayer::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASurvivePlayer::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ASurvivePlayer::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASurvivePlayer::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ASurvivePlayer::OnToggleCamera()
{
	if (ThirdPersonCameraComponent->IsActive()) {
		// Switch to first person
		MeshThirdPerson->SetHiddenInGame(true);
		MeshThirdPerson->SetVisibility(false);

		Mesh1P->SetHiddenInGame(false);
		Mesh1P->SetVisibility(true);

		FirstPersonCameraComponent->SetActive(true);
		ThirdPersonCameraComponent->SetActive(false);

		Weapon->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
		SecondaryWeapon->SetHideMesh(true);
	}
	else {
		// Switch to third person

		MeshThirdPerson->SetHiddenInGame(false);
		MeshThirdPerson->SetVisibility(true);

		Mesh1P->SetHiddenInGame(true);
		Mesh1P->SetVisibility(false);

		ThirdPersonCameraComponent->SetActive(true);
		FirstPersonCameraComponent->SetActive(false);

		Weapon->AttachToComponent(MeshThirdPerson, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
		SecondaryWeapon->SetHideMesh(false);
	}
}

