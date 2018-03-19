// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SurviveWeaponBase.generated.h"

UCLASS(abstract)
class SURVIVE_API ASurviveWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASurviveWeaponBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Call to fire the weapon */
	virtual void Fire();

	/** Set hide mesh */
	virtual void SetHideMesh(const bool hide);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Weapon mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	USkeletalMeshComponent* m_MeshComponent;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	TSubclassOf<UDamageType> DamageType;
};
