// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerAttributeWidget.h"
#include "Components/ProgressBar.h"

void USPlayerAttributeWidget::UpdateHealthBar(const float Value) const
{
	UE_LOG(LogTemp, Display, TEXT("Health Bar Updated: %f%%"), Value);
	HealthBar->SetPercent(Value);
}
