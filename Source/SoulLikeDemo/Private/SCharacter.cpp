// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"

ASCharacter::ASCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	WalkSpeed = 600.0f;

	SprintSpeed = 1000.0f;
	SprintBufferTime = 0.5f;

	RollDistance = 500.0f;
	RollDuration = 0.5f;

	LowestAngle = -30.0f;
	HighestAngle = 30.0f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;

	bCanSprint = true;
	bIsSprinting = false;
	bIsRolling = false;

	PrimaryActorTick.bCanEverTick = true;
}

void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASCharacter::Look);

		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ASCharacter::SprintStart);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ASCharacter::SprintStop);
	
		EnhancedInputComponent->BindAction(RollingAction, ETriggerEvent::Triggered, this, &ASCharacter::Roll);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASCharacter::Attack);
	}
}

void ASCharacter::Move(const FInputActionValue& Value)
{
	if (bIsRolling) return;
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller)
	{
		const FRotator ControlRotation = Controller->GetControlRotation();
		const FRotator YawRotation = FRotator(0.0f, ControlRotation.Yaw, 0.0f);

		const FRotationMatrix RotationMatrix(YawRotation);
		const FVector ForwardDirection = RotationMatrix.GetUnitAxis(EAxis::X);
		const FVector RightDirection = RotationMatrix.GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();
	if (Controller)
	{
		FRotator CurrentRotation = GetControlRotation();
		float NewPitch = CurrentRotation.Pitch - LookVector.Y;
		if (NewPitch > 180.0f)
		{
			NewPitch -= 360.0f;
		}
		// UE_LOG(LogTemp, Log, TEXT("Angle is: %s."), *FString::SanitizeFloat(NewPitch));
		CurrentRotation.Pitch = FMath::Clamp(NewPitch, LowestAngle, HighestAngle);
		AddControllerYawInput(LookVector.X);
		Controller->SetControlRotation(CurrentRotation);
	}
}

void ASCharacter::SprintStart()
{
	if (bIsRolling || !bCanSprint) return;
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void ASCharacter::SprintStop()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	// Sprint buffer
	GetWorld()->GetTimerManager().SetTimer(SprintBufferTimerHandle, [this]()
	{
		bIsSprinting = false;

		GetWorld()->GetTimerManager().ClearTimer(SprintBufferTimerHandle);
	}, SprintBufferTime, false);
}

void ASCharacter::Roll()
{
	if (!bIsRolling)
	{
		bIsRolling = true;
		UE_LOG(LogTemp, Log, TEXT("Roll."));
		
		FVector Velocity = GetVelocity();
		FVector RollDirection;
		if (Velocity.Size() > 0.0f) {
			RollDirection = Velocity.GetSafeNormal();
		}
		else {
			RollDirection = GetActorForwardVector();
		}
		GetWorld()->GetTimerManager().SetTimer(UpdateRollTimerHandle, [this, RollDirection]() {UpdateRollPosition(RollDirection); }, RollDuration / 50.f, true);

		GetWorld()->GetTimerManager().SetTimer(RollTimerHandle, this, &ASCharacter::EndRoll, RollDuration, false);
	}
}

void ASCharacter::UpdateRollPosition(FVector RollDirection)
{
	if (bIsRolling)
	{
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		FVector StartLocation = GetActorLocation();
		FVector EndLocation = StartLocation + RollDirection * RollDistance * RollDuration / 50.f;
		bool bIsHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams);
		// If it is obligated
		if (!bIsHit) return;
		SetActorLocation(EndLocation, true);
	}
}

void ASCharacter::EndRoll()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		bIsRolling = false;
		UE_LOG(LogTemp, Log, TEXT("EndRoll."));

		GetWorld()->GetTimerManager().ClearTimer(RollTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(UpdateRollTimerHandle);
	}, 0.3f, false);
}

void ASCharacter::Attack()
{
}

