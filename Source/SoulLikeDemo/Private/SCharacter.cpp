// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MovieSceneTracksComponentTypes.h"
#include "SWeaponSword.h"
#include "Chaos/NewtonElasticFEM.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"
#include "SAttributeComponent.h"
#include "Kismet/GameplayStatics.h"

ASCharacter::ASCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	WalkSpeed = 400.0f;

	SprintSpeed = 700.0f;
	SprintBufferTime = 0.5f;

	RollDistance = 600.0f;
	RollDuration = 0.6f;

	LowestAngle = -30.0f;
	HighestAngle = 60.0f;

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

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("Attribute"));
	
	bIsWalking = false;
	bIsSprinting = false;
	bIsRolling = false;
	bIsAttacking = false;
	bCanAttackCombo = true;
	
	bCanSprint = true;
	bCanRoll = true;

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

	AttributeComp->OnDeath.AddDynamic(this, &ASCharacter::Death);

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
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ASCharacter::MoveEnd);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASCharacter::Look);

		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ASCharacter::SprintStart);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ASCharacter::SprintStop);
	
		EnhancedInputComponent->BindAction(RollingAction, ETriggerEvent::Triggered, this, &ASCharacter::RollStart);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASCharacter::Attack);
	}
}

void ASCharacter::Move(const FInputActionValue& Value)
{
	if (bIsRolling || bIsAttacking || bIsHitting) return;
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller)
	{
		bIsWalking = true;
		
		const FRotator ControlRotation = Controller->GetControlRotation();
		const FRotator YawRotation = FRotator(0.0f, ControlRotation.Yaw, 0.0f);

		const FRotationMatrix RotationMatrix(YawRotation);
		const FVector ForwardDirection = RotationMatrix.GetUnitAxis(EAxis::X);
		const FVector RightDirection = RotationMatrix.GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASCharacter::MoveEnd()
{
	bIsWalking = false;
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
	if (bIsRolling || !bCanSprint || bIsAttacking || bIsHitting) return;
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void ASCharacter::SprintStop()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	bIsSprinting = false;
}

void ASCharacter::ResetRoll()
{
	bCanRoll = true;
	UE_LOG(LogTemp, Warning, TEXT("ASCharacter::ResetRoll"));
}

void ASCharacter::RollStart()
{
	if (!bIsRolling && bCanRoll && !bIsAttacking && !bIsHitting)
	{
		bIsRolling = true;
		bCanRoll = false;
		OnRoll.Broadcast();
		UE_LOG(LogTemp, Log, TEXT("Roll. %s"), *GetActorLocation().ToString());
		
		FVector Velocity = GetVelocity();
		FVector RollDirection;
		if (Velocity.Size() > 0.1f) {
			RollDirection = Velocity.GetSafeNormal();
		}
		else {
			RollDirection = GetActorForwardVector();
		}

		float RollStartTime = GetWorld()->GetTimeSeconds();
		FVector RollStartLocation = GetActorLocation();
		GetWorld()->GetTimerManager().SetTimer(
			UpdateRollTimerHandle,
			[this, RollDirection, RollStartLocation, RollStartTime]()
			{
				RollUpdate(RollDirection, RollStartLocation, RollStartTime);
			},
			RollDuration / 60.f,
			true
			);

		GetWorld()->GetTimerManager().SetTimer(
			RollTimerHandle,
			this,
			&ASCharacter::RollEnd,
			RollDuration - 0.2f,
			false
			);

		FTimerHandle ResetRollTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			ResetRollTimerHandle,
			this,
			&ASCharacter::ResetRoll,
			RollDuration,
			false
		);
	}
}

void ASCharacter::RollUpdate(FVector RollDirection, FVector StartLocation, double RollStartTime)
{
	if (bIsRolling)
	{
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);
		
		float ElapsedTime = GetWorld()->GetTimeSeconds() - RollStartTime;
		float Progress = FMath::Clamp(ElapsedTime / RollDuration, 0.f, 1.f);
		
		const float DistanceThisFrame = RollDistance * Progress;
		const FVector EndLocation = StartLocation + RollDirection * DistanceThisFrame;

		bool bIsHit = GetWorld()->LineTraceSingleByChannel(
		    HitResult,
		    GetActorLocation(),
		    EndLocation,
		    ECC_Visibility,
		    CollisionParams
		);
		// If it is obligated
		if (bIsHit) return;
		SetActorLocation(EndLocation, true);
		// UE_LOG(LogTemp, Log, TEXT("%s"), *EndLocation.ToString());
	}
}

void ASCharacter::RollEnd()
{
	bIsRolling = false;
	UE_LOG(LogTemp, Log, TEXT("EndRoll. %s"), *GetActorLocation().ToString());

	GetWorld()->GetTimerManager().ClearTimer(RollTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(UpdateRollTimerHandle);
	
}

void ASCharacter::Attack()
{
	if (bCanAttackCombo && !bIsRolling && !bIsHitting)
	{
		bIsAttacking = true;
		bCanAttackCombo = false;
		SprintStop();
		OnAttack.Broadcast();
		UE_LOG(LogTemp, Log, TEXT("Attack"));
	}
}

void ASCharacter::Hit(float Damage, float Strength)
{
	bIsHitting = true;
	OnHit.Broadcast(Damage, Strength);
	AttributeComp->DecreaseHealth(Damage);
}

void ASCharacter::Death()
{
	UE_LOG(LogTemp, Log, TEXT("Death"));
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("L_Map1"));
}
