// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "SCharacter.generated.h"

UCLASS()
class SOULLIKEDEMO_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ASCharacter();

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float WalkSpeed;
	// Sprint variables
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintBufferTime;
	// Roll variables
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RollDistance;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RollDuration;
	
	// Camera variables
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float LowestAngle;
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float HighestAngle;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* RollingAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* AttackAction;
	
	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	UPROPERTY(BlueprintReadOnly)
	bool bIsMoving;
	UPROPERTY(BlueprintReadOnly)
	bool bIsAttacking;
	UPROPERTY(BlueprintReadOnly)
	bool bIsSprinting;
	UPROPERTY(BlueprintReadOnly)
	bool bIsRolling;

	FTimerHandle SprintBufferTimerHandle;
	void SprintStart();
	void SprintStop();
	bool bCanSprint;

	FTimerHandle RollTimerHandle;
	FTimerHandle UpdateRollTimerHandle;
	void Roll();
	void UpdateRollPosition(FVector RollDirection);
	void EndRoll();

	void Attack();
};
