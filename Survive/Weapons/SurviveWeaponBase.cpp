// Fill out your copyright notice in the Description page of Project Settings.

#include "SurviveWeaponBase.h"


// Sets default values
ASurviveWeaponBase::ASurviveWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASurviveWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASurviveWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASurviveWeaponBase::Fire()
{

}

void ASurviveWeaponBase::SetHideMesh(const bool hide)
{
	m_MeshComponent->SetHiddenInGame(hide);
	m_MeshComponent->SetVisibility(!hide);
}