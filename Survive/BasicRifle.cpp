// Fill out your copyright notice in the Description page of Project Settings.

#include "BasicRifle.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABasicRifle::ABasicRifle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
}

void ABasicRifle::Fire()
{
	AActor* owner = GetOwner();
 	if (!owner)
		return;

	FVector eyeLocation;
	FRotator eyeRotation;
	owner->GetActorEyesViewPoint(eyeLocation, eyeRotation);

	FVector shotDirection = eyeRotation.Vector();

	FVector traceEnd = eyeLocation + (shotDirection * 10000);

	/*FCollisionQueryParams queryParams;
	queryParams.AddIgnoreActor(owner);
	queryParams.AddIgnoreActor(this);
	queryParams.bTraceComplex = true;*/

	FHitResult hit;
	if (GetWorld()->LineTraceSingleByChannel(hit, eyeLocation, traceEnd, ECC_Visibility/*, queryParams*/)) {
		// Hit, process damage

		AActor* hitActor = hit.GetActor();
		UGameplayStatics::ApplyPointDamage(hitActor, 20.f, shotDirection, hit, owner->GetInstigatorController(), this, DamageType);
	}

	if (FireSound)
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());

	DrawDebugLine(GetWorld(), eyeLocation, traceEnd, FColor::Red, false, 1.f, 0, 1.f);
}

// Called when the game starts or when spawned
void ABasicRifle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABasicRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

