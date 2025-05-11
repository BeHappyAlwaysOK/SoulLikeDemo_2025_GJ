// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULLIKEDEMO_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	USAttributeComponent();
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Attribute")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Attribute")
	int MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Attribute")
	float Toughness;

	UPROPERTY()
	class USPlayerAttributeWidget* PlayerAttributeWidgetInstance;
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<USPlayerAttributeWidget> PlayerAttributeWidgetClass;

	void BeginPlay() override;

public:
	
	UPROPERTY(BlueprintAssignable)
	FOnDeathDelegate OnDeath;
	
	void DecreaseHealth(float Value);
	
	void UpdatePlayerStatus() const;

};
