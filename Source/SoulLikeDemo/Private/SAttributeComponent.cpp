// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"
#include "SPlayerAttributeWidget.h"
#include "Blueprint/UserWidget.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	MaxHealth = 100;
	Health = MaxHealth;

	Toughness = 5;
}

void USAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	if (PlayerAttributeWidgetInstance == nullptr && ensure(PlayerAttributeWidgetClass))
	{
		PlayerAttributeWidgetInstance = CreateWidget<USPlayerAttributeWidget>(GetWorld(), PlayerAttributeWidgetClass);
		if (PlayerAttributeWidgetInstance)
		{
			PlayerAttributeWidgetInstance->AddToViewport();
			UpdatePlayerStatus();
		}
	}
}

void USAttributeComponent::DecreaseHealth(float Value)
{
	Health = FMath::Clamp(Health - Value, 0, MaxHealth);
	UpdatePlayerStatus();
	if (Health <= 0.0f)
	{
		OnDeath.Broadcast();
	}
}

void USAttributeComponent::UpdatePlayerStatus() const
{
	PlayerAttributeWidgetInstance->UpdateHealthBar(Health / MaxHealth);
}
