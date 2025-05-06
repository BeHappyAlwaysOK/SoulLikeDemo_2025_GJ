// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeaponSword.generated.h"

UCLASS()
class SOULLIKEDEMO_API ASWeaponSword : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeaponSword();

	UFUNCTION(BlueprintImplementableEvent)
	void Attack();

	UPROPERTY(EditAnywhere)
	float Damage;

};
