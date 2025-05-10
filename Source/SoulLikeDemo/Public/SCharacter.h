// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "SCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRollDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHitDelegate, float, Damage, float, Strength);

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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USAttributeComponent* AttributeComp;
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void MoveEnd();

	UPROPERTY(BlueprintReadOnly)
	bool bIsWalking;
	UPROPERTY(BlueprintReadOnly)
	bool bIsSprinting;
	UPROPERTY(BlueprintReadOnly)
	bool bIsRolling;
	
	void SprintStart();
	void SprintStop();
	bool bCanSprint;

	bool bCanRoll;
	FTimerHandle RollTimerHandle;
	FTimerHandle UpdateRollTimerHandle;
	UPROPERTY(BlueprintAssignable)
	FOnRollDelegate OnRoll;
	void ResetRoll();
	void RollStart();
	void RollUpdate(FVector RollDirection, FVector StartLocation, double RollStartTime);
	void RollEnd();


	UPROPERTY(BlueprintReadWrite)
	bool bIsAttacking;
	UPROPERTY(BlueprintReadWrite)
	bool bCanAttackCombo;
	UPROPERTY(BlueprintAssignable)
	FOnAttackDelegate OnAttack;

	UFUNCTION(BlueprintCallable)
	void Attack();

	UPROPERTY(BlueprintAssignable)
	FOnHitDelegate OnHit;

	UPROPERTY(BlueprintReadWrite)
	bool bIsHitting;
	UFUNCTION(BlueprintCallable)
	void Hit(float Damage, float Strength);

	UFUNCTION()
	void Death();

private:
	FHitResult RollHitResult;
	bool bHitObstacleDuringRoll;
};
