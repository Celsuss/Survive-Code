// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/SurviveWeaponBase.h"
#include "BasicRifle.generated.h"

UCLASS()
class SURVIVE_API ABasicRifle : public ASurviveWeaponBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasicRifle();

	/** Call to fire the weapon */
	virtual void Fire();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
