// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SPlayerAttributeWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKEDEMO_API USPlayerAttributeWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

public:
	
	UFUNCTION()
	void UpdateHealthBar(const float Value) const;
	
};
